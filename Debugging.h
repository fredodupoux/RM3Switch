
// debug options //
void debugPrintOut() {
  Terminal.print("\n------------------");
  Terminal.print("\nConnected to ");
  Terminal.print(WiFi.SSID());
  Terminal.print("\nIP address: ");
  Terminal.print(WiFi.localIP());
  Terminal.print("\n" + local.dateTime());
  Terminal.print("\n------------------\n");
  Terminal.flush();
  // 
}