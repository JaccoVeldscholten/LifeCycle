import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';

import '../model/Device.dart';

class MyDevices extends StatelessWidget {
  static const String _title = 'My Devices';

  @override
  Widget build(BuildContext context) {
    // Calling first of all the init
    return MaterialApp(
      title: _title,
      home: Scaffold(
        appBar: AppBar(title: const Text(_title), backgroundColor: Colors.green,),
        body: MyDevicesApp(),
      ),
    );
  }
}

/// This is the stateless widget that the main application instantiates.
class MyDevicesApp extends StatelessWidget {
  final FirebaseFirestore firestore = FirebaseFirestore.instance;
  var messages;

  List devices = [];

  readMyDevices() async {
    messages = await firestore.collection('devices').get();

    for (var m in messages.docs) {
      Device d = new Device.simple(m.data()['name'], m.data()['battery']);
      devices.add(d);
      devices.forEach((element) {
        print("Device name: " +
            element.name +
            " Battery: " +
            element.battery.toString());
      });
    }

    if(messages != null){
      print("There are no devices by this user. Display button for adding them");
    }


    return messages;
  }

  List<Widget> prepareCardWidgets() {
    //here you can do any processing you need as long as you return a list of ```Widget```.
    List<Widget> widgets = [];
    readMyDevices();
    devices.forEach((element) {
      widgets.add(Card(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            ListTile(
              leading: Icon(Icons.devices),
              title: Text(element.name + " Battery: " + element.battery.toString() + "%"),
              subtitle: Text("Last time online: // Need to te done \n"
                  "Last alarm: // Need to te done"),
              onTap: () => print("Needs implementation"),
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.end,
              children: <Widget>[
                Icon(Icons.settings, color: Colors.green,),
                TextButton(
                  child: const Text('SETTINGS', style: TextStyle(color:Colors.green),),
                  onPressed: () {/* ... */},
                ),
              ],
            ),
          ],
        ),
      ));
    });

    return widgets;
  }

  @override
  Widget build(BuildContext context) {
    return Container(

        child: Column(
          children: [
            FutureBuilder(
                future: readMyDevices(),
                builder: (BuildContext context, AsyncSnapshot res) {
                  if (messages == null) {
                    return Container(
                      child: Text('this is nice'), // Loading Icon
                    );
                  }
                  return Container(
                      child: Column(
                        children: prepareCardWidgets(),
                      ));
                }),
          ],
        )

    );
  }
}
