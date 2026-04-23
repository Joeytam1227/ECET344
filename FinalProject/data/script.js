var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad() {
  initWebSocket();
}

function initWebSocket() {
  console.log('Opening WebSocket connection...');
  websocket = new WebSocket(gateway);

  websocket.onopen = function() {
    console.log('WebSocket connection opened');
  };

  websocket.onclose = function() {
    console.log('WebSocket connection closed');
    setTimeout(initWebSocket, 2000);
  };

  websocket.onmessage = function(event) {
    console.log('Message from ESP32:', event.data);
  };
}

function sendMotorCommand(motorNumber, direction) {
  let message = motorNumber + ":" + direction;
  console.log("Sending:", message);
  websocket.send(message);
}