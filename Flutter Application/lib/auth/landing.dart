import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_messaging/firebase_messaging.dart';

class LoginLanding extends StatefulWidget {
  @override
  _LoginLanding createState() => _LoginLanding();
}

class _LoginLanding extends State<LoginLanding> {
  String _email;
  String _firstname;
  String _lastname;
  String _password;
  String _passwordRep;

  final FirebaseFirestore firestore = FirebaseFirestore.instance;
  final FirebaseMessaging _firebaseMessaging = FirebaseMessaging();

  Future<void> _createUser() async {
    Navigator.of(context, rootNavigator: true).pop();
    if (_passwordRep != _password) {
      print('User entered the Repeat password not the same');
      final snackBar = SnackBar(
        content: Text('Whoops! The filled in passwords are not the same'),
        action: SnackBarAction(
          label: 'Undo',
          onPressed: () {},
        ),
      );
      ScaffoldMessenger.of(context).showSnackBar(snackBar);
    } else if (_firstname == null ||
        _lastname == null ||
        _email == null ||
        _password == null) {
      print('User entered not all information');
      final snackBar = SnackBar(
        content: Text('Whoops! Not all information has been entered'),
        action: SnackBarAction(
          label: 'Undo',
          onPressed: () {},
        ),
      );
      ScaffoldMessenger.of(context).showSnackBar(snackBar);
    } else {
      try {
        UserCredential userCredential = await FirebaseAuth.instance
            .createUserWithEmailAndPassword(email: _email, password: _password);
        final User user = FirebaseAuth.instance.currentUser;
        final uid = user.uid;
        try {
          _firebaseMessaging.getToken().then((String token) async {
            await firestore.collection('users').doc(_email).set({
              'firstName': _firstname,
              'lastName': _lastname,
              'email' : _email,
              'token': token,
            });
            print("User Token:" + token);
          });

        } catch (e) {
          print(e);
        }

      } on FirebaseAuthException catch (e) {
        if (e.code == 'weak-password') {
          print('The password provided is too weak.');
          final snackBar = SnackBar(
            content: Text('Whoops! The chosen password is too weak'),
            action: SnackBarAction(
              label: 'Undo',
              onPressed: () {},
            ),
          );
          ScaffoldMessenger.of(context).showSnackBar(snackBar);
        } else if (e.code == 'email-already-in-use') {
          print('The account already exists for that email.');
          final snackBar = SnackBar(
            content: Text('Whoops! The account already exists for that email'),
            action: SnackBarAction(
              label: 'Undo',
              onPressed: () {},
            ),
          );
          ScaffoldMessenger.of(context).showSnackBar(snackBar);
        }
      } catch (e) {
        print(e); // Stacktrace
      }
    }
  }

  Future<void> _login() async {
    Navigator.of(context, rootNavigator: true).pop();
    try {
      UserCredential userCredential = await FirebaseAuth.instance
          .signInWithEmailAndPassword(email: _email, password: _password);
    } on FirebaseAuthException catch (e) {
      if (e.code == 'user-not-found') {
        print('No user found for that email.');
        final snackBar = SnackBar(
          content: Text(
              'Whoops! No user found with that email. Try to register new account'),
          action: SnackBarAction(
            label: 'Undo',
            onPressed: () {},
          ),
        );
        ScaffoldMessenger.of(context).showSnackBar(snackBar);
      } else if (e.code == 'wrong-password') {
        print('User entered wrong password');
        final snackBar = SnackBar(
          content: Text('Whoops! Wrong password filled in'),
          action: SnackBarAction(
            label: 'Undo',
            onPressed: () {},
          ),
        );
        ScaffoldMessenger.of(context).showSnackBar(snackBar);
      }
    }
  }

  void displayRegister(BuildContext context) {
    showModalBottomSheet(
        context: context,
        backgroundColor: Colors.transparent,
        isScrollControlled: true,
        builder: (ctx) {
          return Container(
              height: MediaQuery.of(context).size.height / 1.2,
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(20.0),
              ),
              child: Container(
                decoration: new BoxDecoration(
                  borderRadius: BorderRadius.circular(20.0),
                  color: Colors.white,
                ),
                child: Stack(
                  children: [
                    Container(
                        padding: EdgeInsets.fromLTRB(20, 20, 20, 20),
                        child: Text("Create New Account",
                            style: TextStyle(
                                color: Colors.black,
                                fontSize: 25,
                                fontWeight: FontWeight.bold))),
                    Padding(
                        padding: const EdgeInsets.all(30.0),
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Row(
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: <Widget>[
                                Expanded(
                                  // optional flex property if flex is 1 because the default flex is 1
                                  flex: 1,
                                  child: TextField(
                                    onChanged: (value) {
                                      _firstname = value;
                                    },
                                    decoration: InputDecoration(
                                        prefixIcon: Icon(Icons.person),
                                        labelText: 'Firstname....',
                                        labelStyle:
                                            TextStyle(color: Colors.grey[400])),
                                  ),
                                ),
                                SizedBox(width: 10.0),
                                Expanded(
                                  // optional flex property if flex is 1 because the default flex is 1
                                  flex: 1,
                                  child: TextField(
                                    onChanged: (value) {
                                      _lastname = value;
                                    },
                                    decoration: InputDecoration(
                                        prefixIcon: Icon(Icons.person),
                                        labelText: 'Lastname....',
                                        labelStyle:
                                            TextStyle(color: Colors.grey[400])),
                                  ),
                                ),
                              ],
                            ),
                            TextField(
                              onChanged: (value) {
                                _email = value;
                              },
                              decoration: InputDecoration(
                                  prefixIcon: Icon(Icons.email),
                                  labelText: 'Email....',
                                  labelStyle:
                                      TextStyle(color: Colors.grey[400])),
                            ),
                            SizedBox(width: 10.0),
                            TextField(
                              onChanged: (value) {
                                _password = value;
                              },
                              obscureText: true,
                              obscuringCharacter: "•",
                              decoration: InputDecoration(
                                  prefixIcon: Icon(Icons.vpn_key),
                                  labelText: 'Password....',
                                  labelStyle:
                                      TextStyle(color: Colors.grey[400])),
                            ),
                            SizedBox(width: 10.0),
                            TextField(
                              onChanged: (value) {
                                _passwordRep = value;
                              },
                              obscureText: true,
                              obscuringCharacter: "•",
                              decoration: InputDecoration(
                                  prefixIcon: Icon(Icons.vpn_key),
                                  labelText: 'Repeat Password....',
                                  labelStyle:
                                      TextStyle(color: Colors.grey[400])),
                            ),
                            SizedBox(width: 10.0),
                            Container(
                                margin: const EdgeInsets.only(top: 20.0),
                                child: ElevatedButton(
                                    onPressed: _createUser,
                                    child: Container(
                                      padding:
                                          EdgeInsets.fromLTRB(80, 20, 80, 20),
                                      child: Text("Sign Up",
                                          style:
                                              TextStyle(color: Colors.white)),
                                    ),
                                    style: ButtonStyle(
                                      backgroundColor:
                                          MaterialStateProperty.all<Color>(
                                              Colors.greenAccent),
                                      shape: MaterialStateProperty.all<
                                              RoundedRectangleBorder>(
                                          RoundedRectangleBorder(
                                        borderRadius:
                                            BorderRadius.circular(18.0),
                                      )),
                                    )))
                          ],
                        ))
                  ],
                ),
              ));
        });
  }

  void displayLogin(BuildContext context) {
    showModalBottomSheet(
        context: context,
        backgroundColor: Colors.transparent,
        isScrollControlled: true,
        builder: (ctx) {
          return Container(
              height: MediaQuery.of(context).size.height / 1.2,
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(20.0),
              ),
              child: Container(
                decoration: new BoxDecoration(
                  borderRadius: BorderRadius.circular(20.0),
                  color: Colors.white,
                ),
                child: Stack(
                  children: [
                    Container(
                        padding: EdgeInsets.fromLTRB(20, 20, 20, 20),
                        child: Text("Welcome back!",
                            style: TextStyle(
                                color: Colors.black,
                                fontSize: 25,
                                fontWeight: FontWeight.bold))),
                    Padding(
                        padding: const EdgeInsets.all(30.0),
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            TextField(
                              onChanged: (value) {
                                _email = value;
                              },
                              decoration: InputDecoration(
                                  prefixIcon: Icon(Icons.email),
                                  labelText: 'Email....',
                                  labelStyle:
                                  TextStyle(color: Colors.grey[400])),
                            ),
                            SizedBox(width: 10.0),
                            TextField(
                              onChanged: (value) {
                                _password = value;
                              },
                              obscureText: true,
                              obscuringCharacter: "•",
                              decoration: InputDecoration(
                                  prefixIcon: Icon(Icons.vpn_key),
                                  labelText: 'Password....',
                                  labelStyle:
                                  TextStyle(color: Colors.grey[400])),
                            ),
                            Container(
                                margin: const EdgeInsets.only(top: 20.0),
                                child: ElevatedButton(
                                    onPressed: _login,
                                    child: Container(
                                      padding:
                                          EdgeInsets.fromLTRB(80, 20, 80, 20),
                                      child: Text("Sign in",
                                          style:
                                              TextStyle(color: Colors.white)),
                                    ),
                                    style: ButtonStyle(
                                      backgroundColor:
                                          MaterialStateProperty.all<Color>(
                                              Colors.greenAccent),
                                      shape: MaterialStateProperty.all<
                                              RoundedRectangleBorder>(
                                          RoundedRectangleBorder(
                                        borderRadius:
                                            BorderRadius.circular(18.0),
                                      )),
                                    )))
                          ],
                        ))
                  ],
                ),
              ));
        });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        child: Stack(
          children: [
            Container(
              decoration: BoxDecoration(
                image: DecorationImage(
                  image: AssetImage("assets/login_back.jpg"),
                  fit: BoxFit.cover,
                ),
              ),
            ),
            Container(
                padding: EdgeInsets.fromLTRB(20, 60, 20, 20),
                child: Text(
                  "LifeCycle",
                  style: TextStyle(
                      color: Colors.greenAccent,
                      fontSize: 50,
                      fontWeight: FontWeight.bold),
                )),
            Container(
                padding: EdgeInsets.fromLTRB(20, 130, 20, 20),
                child: Text(
                  "We take care for falling damage",
                  style: TextStyle(color: Colors.greenAccent, fontSize: 30),
                )),
            Center(
                child: Container(
                    margin: const EdgeInsets.only(top: 400.0),
                    child: ConstrainedBox(
                        constraints:
                            BoxConstraints.tightFor(width: 300, height: 70),
                        child: ElevatedButton(
                            onPressed: () => displayRegister(context),
                            child: Container(
                              padding: EdgeInsets.fromLTRB(20, 20, 20, 20),
                              child: Text("Sign up"),
                            ),
                            style: ButtonStyle(
                              backgroundColor: MaterialStateProperty.all<Color>(
                                  Colors.orangeAccent),
                              shape: MaterialStateProperty.all<
                                      RoundedRectangleBorder>(
                                  RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(18.0),
                              )),
                            ))))),
            Center(
                child: Container(
                    margin: const EdgeInsets.only(top: 600.0),
                    child: ConstrainedBox(
                        constraints:
                            BoxConstraints.tightFor(width: 300, height: 70),
                        child: ElevatedButton(
                            onPressed: () => displayLogin(context),
                            child: Container(
                              padding: EdgeInsets.fromLTRB(20, 20, 20, 20),
                              child: Text("Sign in",
                                  style: TextStyle(color: Colors.orangeAccent)),
                            ),
                            style: ButtonStyle(
                              backgroundColor: MaterialStateProperty.all<Color>(
                                  Colors.white),
                              shape: MaterialStateProperty.all<
                                      RoundedRectangleBorder>(
                                  RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(18.0),
                              )),
                            ))))),
          ],
        ),
      ),
    );
  }
}
