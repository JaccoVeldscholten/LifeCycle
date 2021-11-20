import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:intl/intl.dart';

import '../model/Contact.dart';

class MyContacts extends StatelessWidget {
  static const String _title = 'My Contacts';

  @override
  Widget build(BuildContext context) {
    // Calling first of all the init
    return MaterialApp(
      title: _title,
      home: Scaffold(
        appBar: AppBar(title: const Text(_title), backgroundColor: Colors.green,),
        body: MyContactsApp(),
      ),
    );
  }
}

/// This is the stateless widget that the main application instantiates.
class MyContactsApp extends StatelessWidget {


  final FirebaseFirestore firestore = FirebaseFirestore.instance;
  var messages;
  var contacts;

  var contactdoc;

  List devices = [];

  readMyDevices() async {
    messages = await firestore.collection('user_contacts').get();

    for (var m in messages.docs) {
      print("Owner (ME):  ${m.data()['owner']} ");
      DocumentSnapshot result =  await m.data()['contact'].get();
      print(result.id);
    }



    if(messages == null){
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
              title: Text(""), // element.email.toString()
              subtitle: Text(""), // element.addedOn.toString()
              onTap: () => print("Needs implementation"),
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.end,
              children: <Widget>[
                Icon(Icons.settings, color: Colors.green,),
                TextButton(
                  child: const Text('OPTIONS', style: TextStyle(color:Colors.green),),
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
