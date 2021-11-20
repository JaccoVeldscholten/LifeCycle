const functions = require('firebase-functions');
const admin = require('firebase-admin');
const express = require('express');
const cors = require('cors');
const app = express();
app.use(cors({
    origin: true
}));

var serviceAccount = require("./permissions.json");
const { onCall } = require('firebase-functions/lib/providers/https');
admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://lifecycle-server.firebaseio.com"
});
const db = admin.firestore();

// https://us-central1-lifecycle-server.cloudfunctions.net/app/api/write
// Request Table (Addressing Table)
// 0 = Alarm            (ex: 0;52.236374;6.837762)
// 0 = Cancel Alarm     (ex: 0;0;0)
// 1 = Battery          (ex: 1;50)
// 2 = Register Device  (ex: 2;01)
// 3 = Error code       (ex: 3;01)
// - Error Codes are defined at front-end/app
// Else = Error code

app.get('/test', (req, res) => {
    return res.status(200).send('API Working Fine! :)');
});


app.post('/api/write', (req, res) => {
    (async () => {
        if (req.body[0].bn != "") {
            // Process the Call data
            var payload = String(Buffer.from(req.body[1].vs, 'hex'));
            var payloadType = payload.charAt(0);
            try {
                if (payloadType == '0') {
                    // Received 0 = Alarm Update
                    console.log("Incoming Alarm for DeviceID: " + req.body[0].bn);

                    const checkDev = await db.collection('devices')
                    .where('deviceID', '==', req.body[0].bn)
                    .get();
                    
                    if (checkDev.empty) {
                        // Device is not recognized
                        return res.status(200).send("False error! Device not recognized");
                    }

                    var loc = payload.split(";"); // Split payload to retreive long/lat
                    if (parseFloat(loc[1]) != 0 && parseFloat(loc[2]) != 0) {

                        const document = db.collection('devices').doc(req.body[0].bn);
                        await document.update({
                            lastOnline: Date.now(),             // Update LastOnline (because last time alarm)
                            lastAlarm: Date.now(),             // Update the last time the device was in alarm
                        });

                        await db.collection('alarms').doc()
                            .create({
                                deviceID: req.body[0].bn, // MAC-Adress
                                source: req.body[0].bt, // Output from Server Hook (KPN)  
                                location: new admin.firestore.GeoPoint(Number.parseFloat(loc[1]), Number.parseFloat(loc[2])),
                                active: 1,
                                alarmDateTime: Date.now() // Date of alarm
                            });
                        return res.status(200).send("Alarm notified");
                    }
                    else {
                        const currentAlarms = await db.collection('alarms')
                            .where('active', '==', 1)                       // Check if turned on
                            .where('deviceID', '==', req.body[0].bn)        // Check mac Adress
                            .get();

                        if (currentAlarms.empty) {
                            // Very Strange that this would accure. 
                            // But for protection of the API its better
                            return res.status(200).send("Alarm Got Canceld Already");
                        }
                        currentAlarms.forEach(doc => {
                            doc.ref.update({
                                active: 0
                            })
                        });

                        return res.status(200).send("Alarm Removed");
                    }
                }

                if (payloadType == '1') {
                    // Received 1 = Battery update
                    console.log("Incoming Battery update for DeviceID: " + req.body[0].bn + " status: " +  parseInt(payload.charAt(2) + payload.charAt(3)));

                    const document = db.collection('devices').doc(req.body[0].bn);
                    await document.update({
                        lastOnline: Date.now(),             // Update the last time the device was online
                        battery: parseInt(payload.charAt(2) + payload.charAt(3))
                    });
                    return res.status(200).send("Battery status updated");
                }

                if (payloadType == '2') {
                    // Received 2 = Register device
                    console.log("Incoming Registration from DeviceID: " + req.body[0].bn);

                    const checkDev = await db.collection('devices')
                        .where('deviceID', '==', req.body[0].bn)
                        .get();
                        
                    if (checkDev.empty == false) {
                        // Device already registerd
                        return res.status(200).send("Device ID: " + req.body[0].bn + " allready registerd");
                    }

                    await db.collection('devices').doc('/' + req.body[0].bn + '/')
                        .create({
                            deviceID: req.body[0].bn, // MAC-Adress
                            source: req.body[0].bt,
                            battery: 0,
                            registerDateTime: Date.now(), // Date of registration
                            lastOnline: 0,
                            lastAlarm: 0,
                            owner: "",
                            alias: ""
                        });
                        
                    return res.status(200).send(req.body[0].bn + " Registerd");
                }
            } catch (error) {
                console.log(error);
                return res.status(500).send(error);
            }
        } else {
            return res.status(500).send("API Access Denied");
        }

    })();
});

app.get('/api/read/:item_id', (req, res) => {
    (async () => {
        try {
            const document = db.collection('items').doc(req.params.item_id);
            let item = await document.get();
            let response = item.data();
            return res.status(200).send(response);
        } catch (error) {
            console.log(error);
            return res.status(500).send(error);
        }
    })();
});

app.get('/api/foo', (req, res) => {
            return res.status(418).send(response); /* Just a little joke ;3 */
});

exports.app = functions.https.onRequest(app);