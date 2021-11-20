import 'dart:ui';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'package:firebase_auth/firebase_auth.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_messaging/firebase_messaging.dart';
import 'package:flutter/foundation.dart';
import 'package:lifecycle/dashboard/mycontacts.dart';

import 'mydevices.dart';
import '../map/map.dart';

class Dashboard extends StatefulWidget {

  @override
  _DasbhoardState createState() => _DasbhoardState();
}

class _DasbhoardState extends State<Dashboard>{

  final FirebaseFirestore firestore = FirebaseFirestore.instance;
  final User user = FirebaseAuth.instance.currentUser;

  final FirebaseMessaging _firebaseMessaging = FirebaseMessaging();


  /*
   This function retrieves the first name of the user
   First it will retrieve the UID and determine the name of the user
   */
  Future<String> _getFirstName() async {
    final uid = user.uid;
    final messages = await firestore.collection('users').doc(uid).get();
    return messages.data()['firstName'];
  }


  @override
  void initState() {
    super.initState();
    _firebaseMessaging.configure(
      onMessage: (Map<String, dynamic> message) async {
        setState(() {
          print("Push Messaging message: $message");
        });
        print("onMessage: $message");
      },
      onLaunch: (Map<String, dynamic> message) async {
        setState(() {
          print("Push Messaging message: $message");
        });
        print("onLaunch: $message");
      },
      onResume: (Map<String, dynamic> message) async {
        setState(() {
          // User resumed
          print("(On Resume) : Push Messaging message: $message");
        });
        print("Testeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeen");
        Navigator.push(
          context,
          MaterialPageRoute(builder: (context) => ShowMap()),
        );
        print("onResume: $message");
      },
    );

    _firebaseMessaging.getToken().then((String token) {
      assert(token != null);
      setState(() {
        print("Push Messaging token: $token");
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(

      /*
        floatingActionButton: FloatingActionButton(
            onPressed: () => debugPrint("Onpressed!"),
            backgroundColor: Colors.lightGreen,
            child: Icon(Icons.call_missed)),
       */
        body: Stack(
          children: <Widget>[
            Image(
              image: AssetImage("assets/background.jpg"),
              width: MediaQuery.of(context).size.width,
              height: MediaQuery.of(context).size.height,
              fit: BoxFit.cover,
            ),

            Container(
                margin: const EdgeInsets.only(left: 10.0, top: 30.0),
                child: Column(
                  children: [
                    Text("LifeCycle",
                        style: TextStyle(
                            color: Colors.white,
                            fontFamily: 'Arial',
                            fontSize: 50)),
                    /*
                Text("Saving Lifes By Detecting them", style: TextStyle(
                    color: Colors.white,
                    fontSize: 10
                ))
                 */
                  ],
                )),

            // Display Welcome Text
            Container(
                margin: const EdgeInsets.only(left: 20.0, top: 100.0),
                alignment: Alignment.topLeft,
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    FutureBuilder(
                        future: _getFirstName(),
                        builder:
                            (BuildContext context, AsyncSnapshot<String> text) {
                          return new Text(' Hello ${text.data}',
                              style: TextStyle(
                                  fontSize: 20,
                                  fontFamily: 'Roboto',
                                  color: Colors.white));
                        })
                  ],
                )),

            // Display Fall Detection Alarm is there is one
            /*
            Center(
                child: Card(
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(20),
                  ),

              child: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  const ListTile(
                    leading: Icon(Icons.alarm),
                    title: Text('Fall Detection Alarm',
                        style: TextStyle(fontSize: 25)),
                    subtitle: Text('LifeCycle Detected an Fall Alarm'),
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.end,
                    children: <Widget>[
                      TextButton(
                        child: const Text('IGNORE',
                            style: TextStyle(color: Colors.green)),
                        onPressed: () => debugPrint("No info yet"),
                      ),
                      const SizedBox(width: 8),
                      TextButton(
                        child: const Text('VIEW LOCATION',
                            style: TextStyle(color: Colors.red)),
                        onPressed: () => debugPrint("No info yet"),
                      ),
                      const SizedBox(width: 8),
                    ],
                  ),
                ],
              ),
            )),
             */
            // Options for User on Dashboard
            Container(
              margin: EdgeInsets.only(top: 300, bottom: 0),
              alignment: Alignment.bottomCenter,
              child: GridView.count(
                padding: EdgeInsets.all(40),
                crossAxisCount: 2,
                children: [
                  // My Devices
                  Card(
                    margin: EdgeInsets.symmetric(vertical: 10, horizontal: 10),
                    shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20)),
                    child: InkWell(
                      splashColor: Colors.blue.withAlpha(30),
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => MyDevices()),
                        );

                      },
                      child: SizedBox(
                          width: 100,
                          height: 200,
                          child: Column(
                            children: [
                              Container(
                                  margin: EdgeInsets.only(top: 20, bottom: 0),
                                  child: Icon(
                                    Icons.devices,
                                    color: Colors.green,
                                    size: 60,
                                  )),
                              Container(
                                  child: Text("My Devices",
                                      style: TextStyle(
                                          color: Colors.green,
                                          fontWeight: FontWeight.bold,
                                          fontSize: 15))),
                            ],
                          )),
                    ),
                  ),
                  // My Contacts
                  Card(
                    margin: EdgeInsets.symmetric(vertical: 10, horizontal: 10),
                    shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20)),
                    child: InkWell(
                      splashColor: Colors.blue.withAlpha(30),
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => MyContacts()),
                        );

                      },
                      child: SizedBox(
                          width: 100,
                          height: 200,
                          child: Column(
                            children: [
                              Container(
                                  margin: EdgeInsets.only(top: 20, bottom: 0),
                                  child: Icon(
                                    Icons.person_sharp,
                                    color: Colors.green,
                                    size: 60,
                                  )),
                              Container(
                                  child: Text("My Contacts",
                                      style: TextStyle(
                                          color: Colors.green,
                                          fontWeight: FontWeight.bold,
                                          fontSize: 15))),
                            ],
                          )),
                    ),
                  ),
                  //
                  Card(
                    margin: EdgeInsets.symmetric(vertical: 10, horizontal: 10),
                    shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20)),
                    child: InkWell(
                      splashColor: Colors.blue.withAlpha(30),
                      onTap: () {
                        Navigator.pushNamed(context, '/map'); // Debug mappie
                      },
                      child: SizedBox(
                          width: 100,
                          height: 200,
                          child: Column(
                            children: [
                              Container(
                                  margin: EdgeInsets.only(top: 20, bottom: 0),
                                  child: Icon(
                                    Icons.message,
                                    color: Colors.green,
                                    size: 60,
                                  )),
                              Container(
                                  child: Text("Notifications",
                                      style: TextStyle(
                                          color: Colors.green,
                                          fontWeight: FontWeight.bold,
                                          fontSize: 15))),
                            ],
                          )),
                    ),
                  ),

                  Card(
                    margin: EdgeInsets.symmetric(vertical: 10, horizontal: 10),
                    shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20)),
                    child: InkWell(
                      splashColor: Colors.blue.withAlpha(30),
                      onTap: () {
                        Navigator.pushNamed(context, '/mydevices');
                      },
                      child: SizedBox(
                          width: 100,
                          height: 200,
                          child: Column(
                            children: [
                              Container(
                                  margin: EdgeInsets.only(top: 20, bottom: 0),
                                  child: Icon(
                                    Icons.settings,
                                    color: Colors.green,
                                    size: 60,
                                  )),
                              Container(
                                  child: Text("Settings",
                                      style: TextStyle(
                                        color: Colors.green,
                                        fontWeight: FontWeight.bold,
                                        fontSize: 15,
                                      ))),
                            ],
                          )),
                    ),
                  ),

                  TextButton(
                    onPressed: () async {
                      await FirebaseAuth.instance.signOut();
                    },
                    child: Text('Debug Logout button'),
                  )
                ],
              ),
            ),
          ],
        ));
  }

}

