import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_messaging/firebase_messaging.dart';

import 'dashboard/dashboard.dart';
import 'auth/landing.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: LandingPage(),
    );
  }
}

class LandingPage extends StatelessWidget{


  final Future<FirebaseApp> _init = Firebase.initializeApp();

  Widget build(BuildContext context){
    return FutureBuilder(
        future: _init,
        builder: (context, snapshot){
          if(snapshot.hasError){
            return Scaffold(
              body: Center(
                child: Text("Error: ${snapshot.error}"),
              ),
            );
          }

          if(snapshot.connectionState == ConnectionState.done){

            return StreamBuilder(
                stream: FirebaseAuth.instance.authStateChanges(),
                builder: (context, snapshot){
                  if(snapshot.connectionState == ConnectionState.active){

                    User user = snapshot.data;

                    if(user == null){
                      return LoginLanding();
                    }
                    else{
                      return Dashboard();
                    }
                  }

                  return Scaffold(
                      body: Center(
                        child: Text("Checking authentication....."),
                      )
                  );
                }

            );
          }


          return Scaffold(
              body: Center(
                child: Text("Connecting to app......"),
              )
          );


        }
    );
  }
}