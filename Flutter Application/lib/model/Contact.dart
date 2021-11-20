class Contact{

  String email;
  String firstName;
  String lastName;
  String addedOn;


  Contact(String email, String firstname, String lastname, DateTime addedOn){
    this.email = email;
    this.firstName = firstName;
    this.lastName = lastname;
    this.addedOn = addedOn as String;
  }

  Contact.simple(String email, String addedOn)
  {
    this.email = email;
    this.addedOn = addedOn;
  }

}
