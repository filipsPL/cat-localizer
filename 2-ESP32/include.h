void sendData(String preambula, float val){

  char result[6]; // Buffer big enough for 7-character float
  //sprintf(result, "%-3.1f", val);
  sprintf(result, "%-3.2f", val);

  HTTPClient http;

  String serverPath = apiurl + String(preambula) + String("+value=") + String(result);;
  //  Serial.print(serverPath);
  
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());


// Send HTTP GET request
int httpResponseCode = http.GET();

if (httpResponseCode>0) {
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  String payload = http.getString();
  Serial.println(payload);
}
else {
  Serial.print("Error code: ");
  Serial.println(httpResponseCode);
  Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpResponseCode).c_str());
}
// Free resources
http.end();

}




// --------------------------------------------------------------------- //


String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  Serial.println(serverName);
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
   http.end();

  return String(payload);
// return String("OK!");
}

// --------------------------------------------------------------------- //
