
// Import the functions you need from the SDKs you need
//import { initializeApp } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-app.js";
//import { getAnalytics } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-analytics.js";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyBZm508LFKd2G63sqmXYgy-QL_4a-ZP6hY",
  authDomain: "finalproject-1c468.firebaseapp.com",
  databaseURL: "https://finalproject-1c468-default-rtdb.firebaseio.com",
  projectId: "finalproject-1c468",
  storageBucket: "finalproject-1c468.appspot.com",
  messagingSenderId: "219962814242",
  appId: "1:219962814242:web:1a84fb7f274772dbd97f7b",
  //measurementId: "G-9T1RXMQMW1"
}
// Initialize Firebase
// 

  firebase.initializeApp(firebaseConfig);

  var database = firebase.database();
  
  function isChecked(){
    var p = document.getElementById("toggle").checked;
    database.ref().update({
      "pump_web": Number(p)
    })
  }

  function feed_onClick(){
    var w = document.getElementById("feed_input").value;
    //console.log(w);
    database.ref().update({
        "feed_web": Number(w)
    })
  }

  function light_onClick(){
    var l = document.getElementById("light_input").value;
    //console.log(w);
    database.ref().update({
        "Lum": Number(l)
    })
  }

  database.ref("/light").on("value", function(snapshot){
    var led = snapshot.val();
    if(led >= 1){
        document.getElementById("img_light").src = "./image/led_on.png";
    }else{
        document.getElementById("img_light").src = "./image/led_off.png";
    }
})

//Lấy giá trị cảnh báo trộm
database.ref("/thief").on("value", function(snapshot){
  var warn = snapshot.val();
  if(warn <= 0){
      document.getElementById("warn").innerHTML = "Không phát hiện đột nhập. An toàn!";
  }else{
      document.getElementById("warn").innerHTML = "Phát hiện đột nhập";
  }
})

//Lấy giá trị nhiệt độ, độ ẩm
database.ref("/Temperature").on("value", function(snapshot){
  var tem = snapshot.val();
  document.getElementById("sub_tem").innerHTML = tem;
})

database.ref("/Humidity").on("value", function(snapshot){
  var hum = snapshot.val();
  document.getElementById("sub_hum").innerHTML = hum;
})

//Lấy giá trị cường độ sáng
database.ref("/Lum_control").on("value", function(snapshot){
  var inten = snapshot.val();
  document.getElementById("sub_light_control").innerHTML = inten;
})

//Lấy giá trị máy bơm
database.ref("/Pump").on("value", function(snapshot){
  var pump = snapshot.val();
  if(pump >= 1){
      document.getElementById("sub_pump").innerHTML = "Máy bơm đang bật";
  }else{
      document.getElementById("sub_pump").innerHTML = "Máy bơm đang tắt";
  }
})