import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';
import 'package:location/location.dart';
import 'package:flutter_polyline_points/flutter_polyline_points.dart';
import 'package:url_launcher/url_launcher.dart';
import 'package:maps_toolkit/maps_toolkit.dart' as mp;
import 'package:permission_handler/permission_handler.dart';

import 'dart:math' show cos, sqrt, asin;

//https://levelup.gitconnected.com/how-to-add-google-maps-in-a-flutter-app-and-get-the-current-location-of-the-user-dynamically-2172f0be53f6

String pointToPointDistance = '';
const APIKEY = ''; // Googgle API key

class ShowMap extends StatefulWidget {
  @override
  _ShowMapState createState() => _ShowMapState();
}

class _ShowMapState extends State<ShowMap> {
  GoogleMapController mapController;
  static LatLng _alarmLocation =
      const LatLng(52.2364731954253, 6.837936021242661);

  static LatLng _myLocation = LatLng(0, 0);
  static LatLng _myCurrentLocation = LatLng(100, 100);

  Location _location = Location();

  Map<MarkerId, Marker> markers = <MarkerId, Marker>{};

  MarkerId idAlarm = MarkerId("Alarm Location");
  MarkerId idMySelf = MarkerId("Own Location");

  bool permissionGPSDenied = false;

  void updateMarkers() {
    Marker _markerAlarm = Marker(
        markerId: idAlarm,
        position: _alarmLocation,
        infoWindow: InfoWindow(
            title: "Fall Detect Location", snippet: "Alarm Reported on: "),
        icon: BitmapDescriptor.defaultMarkerWithHue(BitmapDescriptor.hueRed));

    Marker _markerMySelf = Marker(
        markerId: idMySelf,
        position: _myLocation,
        infoWindow: InfoWindow(title: "My Location", snippet: "Last update: "),
        icon: BitmapDescriptor.defaultMarkerWithHue(BitmapDescriptor.hueGreen));

    if (mounted) {
      setState(() {
        markers[idAlarm] = _markerAlarm;
        markers[idMySelf] = _markerMySelf;
      });
    }
  }

  Map<PolylineId, Polyline> polylines = {};
  List<LatLng> polylineCoordinates = [];
  PolylinePoints polylinePoints = PolylinePoints();

  Future<void> _addPolyLine() async {
    PolylineId id = PolylineId("poly");
    Polyline polyline = Polyline(
        polylineId: id,
        color: Colors.lightGreen,
        points: polylineCoordinates,
        width: 5);
    polylines[id] = polyline;
    //setState(() {});
  }

  void checkPermission() async {
    var status = await Permission.location.status;
    if (status.isDenied) {
      permissionGPSDenied = true;
      final snackBar = SnackBar(
        content: Text(
            'No Permissions granted for GPS. A lot of functionality is lost. Change the settings in app settings'),
        action: SnackBarAction(
          label: 'Undo',
          onPressed: () {
            openAppSettings(); // Ask User to change settings
          },
        ),
      );
      ScaffoldMessenger.of(context).showSnackBar(snackBar);
    }
  }

  double _coordinateDistance(lat1, lon1, lat2, lon2) {
    var p = 0.017453292519943295;
    var c = cos;
    var a = 0.5 -
        c((lat2 - lat1) * p) / 2 +
        c(lat1 * p) * c(lat2 * p) * (1 - c((lon2 - lon1) * p)) / 2;
    return 12742 * asin(sqrt(a));
  }

  Future<void> _getPolyline() async {
    polylineCoordinates.clear();
    PolylineResult result = await polylinePoints.getRouteBetweenCoordinates(
        APIKEY,
        PointLatLng(_myLocation.latitude, _myLocation.longitude),
        PointLatLng(_alarmLocation.latitude, _alarmLocation.longitude),
        travelMode: TravelMode.driving);
    if (result.points.isNotEmpty) {
      result.points.forEach((PointLatLng point) {
        polylineCoordinates.add(LatLng(point.latitude, point.longitude));
      });
    }
    _addPolyLine();

    double totalDistance = 0.0;

    // Calculating the total distance by adding the distance
    // between small segments
    for (int i = 0; i < polylineCoordinates.length - 1; i++) {
      totalDistance += _coordinateDistance(
        polylineCoordinates[i].latitude,
        polylineCoordinates[i].longitude,
        polylineCoordinates[i + 1].latitude,
        polylineCoordinates[i + 1].longitude,
      );
    }

    if (mounted) {
      setState(() {
        pointToPointDistance = totalDistance.toStringAsFixed(2);
        debugPrint('DISTANCE: $pointToPointDistance km');
      });
    }
  }

  // Current Location User
  Future<void> _onMapCreate(GoogleMapController controller) async {
    checkPermission(); // Await Permissions
    mapController = controller;
    _location.onLocationChanged.listen((l) {
      _myLocation = LatLng(l.latitude, l.longitude);
      updateMarkers();
      print("update");
      // _getPolyline(); //<-- Uncomment to change dynamically

      if (_myLocation != _myCurrentLocation) {
        // User Location had changed update the route
        _myCurrentLocation = _myLocation;
      }
    });
  }

  Future<void> _goToAlarm() async {
    CameraPosition alarmPosition =
        CameraPosition(target: _alarmLocation, zoom: 17.0);
    final GoogleMapController controller = mapController;
    controller.animateCamera(CameraUpdate.newCameraPosition(alarmPosition));
  }

  Future<void> _goToMyself() async {
    CameraPosition myLocation = CameraPosition(target: _myLocation, zoom: 17.0);
    final GoogleMapController controller = mapController;
    controller.animateCamera(CameraUpdate.newCameraPosition(myLocation));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      /*
      appBar: AppBar(
        backgroundColor: Colors.lightGreen,
        title: Text("Alarm Location Time: "),
      ),
       */
      body: Stack(
        children: [
          Container(
            child: GoogleMap(
                markers: Set<Marker>.of(markers.values),
                polylines: Set<Polyline>.of(polylines.values),
                onMapCreated: _onMapCreate,
                initialCameraPosition:
                    CameraPosition(target: _alarmLocation, zoom: 17.0)),
          ),
          Positioned(
              left: 20,
              top: 60,
              child: FloatingActionButton.extended(
                heroTag: "MoveToAlarmBtn",
                onPressed: _goToAlarm,
                label: Text("Alarm"),
                icon: Icon(Icons.warning),
                backgroundColor: Colors.red,
              )),
          Positioned(
              left: 150,
              top: 60,
              child: FloatingActionButton.extended(
                heroTag: "MoveToMySelfBtn",
                onPressed: _goToMyself,
                label: Text("Myself"),
                icon: Icon(Icons.info),
                backgroundColor: Colors.lightGreen,
              )),
          Positioned(
              left: 290,
              top: 60,
              child: FloatingActionButton.extended(
                heroTag: "CallAlarmBtn",
                label: Text('Call'),
                onPressed: () => debugPrint("ToDo: Calling"),
                icon: Icon(Icons.phone),
                backgroundColor: Colors.lightBlue,
              )),
          Positioned(
              left: 20,
              top: 150,
              width: 80,
              // permissionGPSDenied == true? FloatingActionButton.extended(
              child: FloatingActionButton.extended(
                heroTag: "DistanceBtn",
                label: Text(pointToPointDistance + "\n km",
                    style: TextStyle(
                      fontWeight: FontWeight.bold,
                    ),
                    textAlign: TextAlign.center),
                onPressed: () {
                  final snackBar = SnackBar(
                    content: Text('Distance to alarm location: ' +
                        pointToPointDistance +
                        " km"),
                    action: SnackBarAction(
                      label: 'Close',
                      onPressed: () {},
                    ),
                  );
                  ScaffoldMessenger.of(context).showSnackBar(snackBar);
                },
                backgroundColor: Colors.lightBlue,
              )),
          Positioned(
            left: 10,
            bottom: 30,
            child: FloatingActionButton(
                onPressed: () => Navigator.pop(context),
                backgroundColor: Colors.lightGreen,
                child: Icon(Icons.arrow_back)),
          ),
        ],
      ),
    );
  }
}
