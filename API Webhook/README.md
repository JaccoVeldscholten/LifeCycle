<img src="https://i.imgur.com/ngv3hMQ.png" width="20%" height="20%" >

# LifeCycle Webhook
This is a webhook for LifeCycle. This webhook processes the information flow from KPN / TTN to the Firebase database. The webhook can be run locally or run on the Firebase Cloud.

Written in NodeJS with Express and Cors


## Compiling
1. Make sure PostMan / NodeJS / NPM / Firebase CLI is installed
2. Run command in CMD: ``` firebase login ```
3. Run command : ```firbase deploy ```

## Install

#### Windows (Tested on Windows 10)
Use Postman to send requests to: https://us-central1-lifecycle-server.cloudfunctions.net/app/api/write
```
// Request Table (Addressing Table)
// 0 = Alarm            (ex: 0;52.236374;6.837762)
// 1 = Battery          (ex: 1;50)
// 2 = Register Device  (ex: 2;01)
// 3 = Error code       (ex: 3;01)
// - Error Codes are defined at front-end/app
// Else = Error code
```

## License
This repo belongs to Team LifeCycle. And there are no rights for using this repo.
