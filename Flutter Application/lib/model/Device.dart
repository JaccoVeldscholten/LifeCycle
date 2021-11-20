class Device{

  String name;
  int bindedTo;   // Might be Redundant
  int battery;
  DateTime lastContact;
  DateTime lastAlarm;

  Device.simple(String name, int battery)
  {
    this.name = name;
    this.battery = battery;
  }

  Device(String name, int battery, DateTime lastContact, DateTime lastAlarm){
    this.name = name;
    this.battery = battery;
    this.lastContact = lastContact;
    this.lastAlarm = lastAlarm;
  }

}
