#include <WiFi.h> //Wifi kniznica - pripojenie na wifi, HTTP konektivita - Webclient
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include "SparkFun_Si7021_Breakout_Library.h" //Kniznica pre SHT21 senzor
const char* ssid = "MY_SSID"; //SSID
const char* pass = "MY_PASS"; //heslo
const char* host = "www.example.com"; //adresa externeho webservera
const int rele = 23; //nastavenie premennej na GPIO pin 23
int pocitadlo = 0; //pocitadlo pre timeout
int rssi;
int led = 18;
Weather sensor; //instancia pre senzor
#ifdef __cplusplus 
extern "C" { //externa cast jazyka C
  #endif
  uint8_t temprature_sens_read(); //systemove volanie funkcie teplomera
  #ifdef __cplusplus
  }
#endif
const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDTzCCAjcCFGoeGOBevdCzZX61CSkNsw1yLOgeMA0GCSqGSIb3DQEBCwUAMGQx\n" \
"CzAJBgNVBAYTAlVTMRMwEQYDVQQIDApGYWtlIFN0YXRlMRYwFAYDVQQHDA1GYWtl\n" \
"IExvY2FsaXR5MRUwEwYDVQQKDAxGYWtlIENvbXBhbnkxETAPBgNVBAMMCGVzcDMy\n" \
"LnNrMB4XDTE4MTIxNDE4MTcyNVoXDTIzMTIxMzE4MTcyNVowZDELMAkGA1UEBhMC\n" \
"VVMxEzARBgNVBAgMCkZha2UgU3RhdGUxFjAUBgNVBAcMDUZha2UgTG9jYWxpdHkx\n" \
"FTATBgNVBAoMDEZha2UgQ29tcGFueTERMA8GA1UEAwwIZXNwMzIuc2swggEiMA0G\n" \
"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDeimA7p/eIaGtXPM2QyD3+tDI+6ltB\n" \
"bPzHUPupBK3EYi1bCtHGg0Lq8cbCdFVhJJnvRGoIdz/knhTIkMphMvTC5UJLooPx\n" \
"RnI9KSdXWs2n/wXYvPS9Txthz6zv0Ua9Dp+OcXvz0wzG1skhd+RPAm9zUJDJ42P1\n" \
"FIjcehxH93GgrHzXz6zklFt01/8IAcBt7gl8/qBnjJNA8SS0BoYgfRnNOvn6k1yk\n" \
"ZZtyfIp68NWlfdveMd8Ig2pUIuvTh7MyUFjumcDLqAs2J+SenUYHEcfZJK4Vmdfo\n" \
"bZam4hx8f0BGrxqDhDMuTmh/Jqa8ZYANfR7ctD1DU6u7aceGYmSgCiNpAgMBAAEw\n" \
"DQYJKoZIhvcNAQELBQADggEBACA5n68AsjspvR3N33jF83FZqDXuvQbmY7lBm2nN\n" \
"Yh2265f9mRGrOO3z6lHKUfs5BJXxxahAKPQ07H9zQuYvIW0y8Jcgs4FlFhAuqgss\n" \
"KWuqJicf/noRPq/MC7mTLSGVrCo3R3x8U842j/Jdi0Ygb7AgFMp5F7DoS88QEMuK\n" \
"2UbJSA0czc24NImg9Y8Ve6VS4m+lsjsl77L15JCuDeZ7UBegmYoDmEIC/MohSD5t\n" \
"goX0+fCH4EsIkd6Zh8P6GTlRCglyGrSFgKTV/6HPSptAFvMFUofnfedCYJXrkjKc\n" \
"psVaEdVBIz/a4jcB6mkQQm06+KusHmwsc/VNYLD6+pjmIE0=\n" \
"-----END CERTIFICATE-----\n";




// You can use x.509 client certificates if you want
const char* test_client_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpQIBAAKCAQEA6pmuN96IQ11+jY25KDBcSab98wGBtSv9W4LVQZVSE2pV2cTh\n" \
"REgpFL8SyFyQ7DdJkHYLbX22LHjd0vy0yzDK3v4+/9Cjo2b1ePDpMAiavF5uhAut\n" \
"9bahsJTr5uSu75RfwEJSI1vE+O0FS3TvNfVpYXocRdmQpvoa6ey5WuJLi8Y1unCi\n" \
"csT54RpwRR5HAC8Sj1B03hp28a4J5ht89vfdPyy004C0kA+fSZVyzoS1ohrv5kHH\n" \
"8MkvTO9KxpHlC7NCuKClL/Vrw6NyC+Fxfz69qN0yahZg5C95ULW1U6BJ6f15v4XY\n" \
"xRA1B3tcJLU9Y+UkKOzEzTew2OQomLkzIY/05wIDAQABAoIBAQCqy+IDuNaZvNqi\n" \
"8fhIYxT9Zj9iiMlG/lTBJ/pcefq8OeFw3x5l2OdXvFhPWN0FrKa5LsfZGfnkXMaa\n" \
"kfKKL+ay/IUfN2W7U/0I5NPQbaNB2ro5Fku7JRCIf5NTkVAvCmW/kkk8htZbqCv/\n" \
"qI1zSk1Dc2RcEi528oVfo0G7Ah+gDPQVU/FSZImZ3Kb/WGtLD2AzL38L0T8PxGQC\n" \
"zKay9Qf8uF7O5pyPgOAPFZlifqZaigfhwpHsyARd+ER8C4k3UwndV07e7CwbTQDh\n" \
"zNgyltF+5toYPWA/LSr20ZWxZPD+C88ScjhEh7D62GPmr5JmX+gxhb0SnCs6X39S\n" \
"pns0TmeZAoGBAPy0H3DmqEK+8Mb2UAdMAOImd/137mz5AL7MUwqHLOAdfgw9w7to\n" \
"+OiUG16dGARdSS29qfpxOcveZ/gVhIj8V9///Rfa3aUbrZkVjhNhsu/CJqPtG2kA\n" \
"Ko+qDhj21CV6aIqWuLWKaKZfC7b6ODLm5zuIIMIST4GPMxnP9akTp9UDAoGBAO2p\n" \
"GosWwrXochRAU9l+2DkZOBZq5bsw4fCjiVS7AbofzszF7vXY05LBZpFV4ePXmrOf\n" \
"8bZw801mxE3Kk0ooBpmwGKEwRRIXXgFcBe8aSKqaidi7+99aCNxjRH4lA+MMMaQn\n" \
"psxDMOzbC8vSyGiSMb2s+K4Of7MTW4jg+T9zCqFNAoGAe/SS05ug/5SPoNjjqqCi\n" \
"ckwyYXe50WRW9EBfn3NtFMYCtVh6sWwtZFB6GG/wfvuPUMuPHFT757/BuhQ7/cHi\n" \
"DPORn2jxffLUtZm4JlOvJSJUN+avolK9jdj5r08sAvJHY2vJUOuCp9FJesjEVluj\n" \
"IzWD9A5tR30JsOisdPI6SicCgYEAxitmhaoR68sKLNbC1C3PsDTWLuMt8lu9WoGS\n" \
"xsM0mnVhZCB7tCX7rXh0xSU+uVSTm8n3rDAcwGK7tz3+eSxM7KQItYdhYMd5TFrm\n" \
"W8yn4RIBdWayjDgwAU0I1MKarh0B26ZwkAxK5IV2HZwCHighL87mgYR3eOPgRI+L\n" \
"ZhvQNlUCgYEA7kyWYdISUClXNdTJKUXbuHsMh/ushv+hLerwvXhA6cLjTetB9ljC\n" \
"evdSS850X/y5Qe2DJIYR/woQh0KDPnOva4PjaLQ7SCUp1DjGvR1vfINgiJyQ4hS+\n" \
"Xi6z101s2PqWmL0I8Kky85wIs+HyjJw+jnRqa37z+8AJIQX9xMUVzK4=\n" \
"-----END RSA PRIVATE KEY-----\n";



const char* test_client_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEIzCCAwugAwIBAgIUDPhu5hnksF+1QHC9qFqLPCv8rHMwDQYJKoZIhvcNAQEL\n" \
"BQAwZDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkZha2UgU3RhdGUxFjAUBgNVBAcM\n" \
"DUZha2UgTG9jYWxpdHkxFTATBgNVBAoMDEZha2UgQ29tcGFueTERMA8GA1UEAwwI\n" \
"ZXNwMzIuc2swHhcNMTgxMjE0MTgxOTEyWhcNMjMxMjEzMTgxOTEyWjBkMQswCQYD\n" \
"VQQGEwJVUzETMBEGA1UECAwKRmFrZSBTdGF0ZTEWMBQGA1UEBwwNRmFrZSBMb2Nh\n" \
"bGl0eTEVMBMGA1UECgwMRmFrZSBDb21wYW55MREwDwYDVQQDDAhlc3AzMi5zazCC\n" \
"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOqZrjfeiENdfo2NuSgwXEmm\n" \
"/fMBgbUr/VuC1UGVUhNqVdnE4URIKRS/EshckOw3SZB2C219tix43dL8tMswyt7+\n" \
"Pv/Qo6Nm9Xjw6TAImrxeboQLrfW2obCU6+bkru+UX8BCUiNbxPjtBUt07zX1aWF6\n" \
"HEXZkKb6GunsuVriS4vGNbpwonLE+eEacEUeRwAvEo9QdN4advGuCeYbfPb33T8s\n" \
"tNOAtJAPn0mVcs6EtaIa7+ZBx/DJL0zvSsaR5QuzQrigpS/1a8OjcgvhcX8+vajd\n" \
"MmoWYOQveVC1tVOgSen9eb+F2MUQNQd7XCS1PWPlJCjsxM03sNjkKJi5MyGP9OcC\n" \
"AwEAAaOBzDCByTCBiwYDVR0jBIGDMIGAoWikZjBkMQswCQYDVQQGEwJVUzETMBEG\n" \
"A1UECAwKRmFrZSBTdGF0ZTEWMBQGA1UEBwwNRmFrZSBMb2NhbGl0eTEVMBMGA1UE\n" \
"CgwMRmFrZSBDb21wYW55MREwDwYDVQQDDAhlc3AzMi5za4IUah4Y4F690LNlfrUJ\n" \
"KQ2zDXIs6B4wCQYDVR0TBAIwADALBgNVHQ8EBAMCBPAwIQYDVR0RBBowGIIIZXNw\n" \
"MzIuc2uCDHd3dy5lc3AzMi5zazANBgkqhkiG9w0BAQsFAAOCAQEAFKU9rRrCBOAj\n" \
"RQ5otniJCPiypK3uO3a4syFSLW0gWueqPx0gH3VBLVbPTCoFb3Lqjg9uWZLsxSOL\n" \
"8iUrdnmsUG8OJl4QYHCDhUa7m+2tReQzlYNRYm46qDutQlMnvNq9Nt8dG+B7uUut\n" \
"0UWpxDalVUVkdc6/58B1ypsMjcHWbJWwVN/LbpmSvKgfZuT+4jYF7VMgmQy6iCa1\n" \
"1KQ5xP1BX86co1vLAK8ZaKACekKsmwApaUVDLZX1kFsjvPXh0pomv3BAWdILclCp\n" \
"W/fPMF885ONcCIsrWJB4f5Y6F7mp5feIN94yaIW2qnfuwTVsPX0vYWx7RhPYPrNk\n" \
"DptkN2Jdaw==\n" \
"-----END CERTIFICATE-----\n";


WiFiClientSecure client;
void setup() { //cast setupu prebieha iba raz
  Serial.begin(115200); //pocet znakov/s v serial monitore - UART
  delay(10); //vyckaj na inicializaciu
  sensor.begin();
  Serial.println();
  Serial.print("Pripajam sa na wifi siet: ");
  Serial.println(ssid); //informativne vypisy o nazve wifi siete
  WiFi.disconnect(true);  //odpoj sa od wifi pred nastavenim noveho pripojenia
  WiFi.mode(WIFI_STA); //nastavenie modu wifi modulu - vyzadovane v ESP32 Arduino Core v poslednych 2 verziach
  //pinMode(LED_BUILTIN, OUTPUT); nastavenie vystupu - zabudovana led
  pinMode(rele, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) { //pokym nie je zariadenie autentizovane a autorizovane, pozdrz program a posielaj . do vypisu
    delay(500); // pauza 500ms
    Serial.print("."); // vypis znaku
    pocitadlo++; //inkrementuj pocitadlo
    if(pocitadlo>=60){ //po 30 sekundach restartuj dosku softverovym resetom, ak nedojde k pripojeniu
      ESP.restart();
    }
  }
      client.setCACert(test_root_ca);
    client.setCertificate(test_client_cert);
    client.setPrivateKey(test_client_key);
  
  
  
  digitalWrite(led, HIGH);
  Serial.println("");
  Serial.println("WiFi pripojene");
  Serial.println("IP addresa nastavena: "); 
  Serial.println(WiFi.localIP()); //vypis pridelenu IP od DHCP sluzby
}

void restart(){
  Serial.print("Pripajam sa na: ");
  Serial.println(host);
  if (client.connect(host, 443)) { //ak je spojenie uspesne na server na porte 80 vykonaj blok prikazov
    Serial.println("Pripojenie pre kontrolu resetu uspesne");  
    String url = "/values/reset.txt"; //citanie z URL adresy na serveri
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
    //vykonanie GET HTTP requestu s verziou HTTP hlavicky, adresa hosta, user-agent zariadenia, ukoncenie HTTP hlavicky
    while (client.connected()) {
      String hlavicka = client.readStringUntil('\n'); //vypis hlavicky po riadkoch, verzia, engine, info o serveri, dlzka response atd...
      Serial.println(hlavicka);
      if (hlavicka == "\r") {
        break; //prazdny riadok za hlavickou, ukonc cyklus
      }
    }
  String premenna = client.readStringUntil('\n'); //riadok za HTTP hlavickou - nasa premenna, s ktorou pracujeme
  Serial.println(premenna);
  if(premenna=="RST"){ //za normalnych okolnosti je v premennej OK, ignoruje sa, pokym z webu nie je vyziadany reset, ktory zmeni obsah na RST.
    Serial.println("Restart vyziadany");
    client.stop(); //ukonc vsetky HTTP spojenia
    if (client.connect(host, 443)) {
      Serial.println("Potvrdzujem reset");
      String url = "/esp32/potvrdreset.php"; //potvrd reset tymto HTTP requestom, ktory opat nastavi do textoveho suboru text OK
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
      delay(1000);
      Serial.println("Restartujem...");
      ESP.restart(); //softverovy restart zariadenia
    }else{
      Serial.println("Nepodarilo sa potvrdit reset - pripojenie zlyhalo");
    }
  }
  }else{
    Serial.println("Nepodarilo sa nacitat stav resetu");
  } 
  client.stop();
}

  void stav_rele(){
 Serial.print("Pripajam sa na: ");
  Serial.println(host);
  if (client.connect(host, 443)) {
    Serial.println("Pripojenie pre stav rele uspesne");
    String url = "/values/stav.txt";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
    while (client.connected()) {
      String hlavicka = client.readStringUntil('\n');
      Serial.println(hlavicka);
      if (hlavicka == "\r") {
        break;
      }
    }
   String premenna = client.readStringUntil('\n');
   Serial.println(premenna);
   if(premenna=="ZAP"){ //rozhodovanie medzi ZAP a VYP stavom citaneho z .txt suboru
    Serial.println("ZAPINAM RELE");
    digitalWrite(rele, LOW);
   // digitalWrite(LED_BUILTIN, HIGH);
   }else if(premenna=="VYP"){
    Serial.println("VYPINAM RELE");
    digitalWrite(rele, HIGH);
   // digitalWrite(LED_BUILTIN, LOW);
   }
  }else{
      Serial.println("Nepodarilo sa nacitat stav rele");
    } 
    client.stop(); //ukonc spojenia
}

void odoslanie(){
   float vlhkost = sensor.getRH();
  float teplotadnu = sensor.getTemp();
  String hum = String(vlhkost);
  String temp2 = String(teplotadnu);
  float teplota = (temprature_sens_read() - 32) / 1.8; //teplota cpu, povodna extern C funkcia - konstanta / konstanta
    String temp = String(teplota); //Pomocna premenna typu String, kolizia pri vyskladani GET requestmi s roznymi datovymi typmi String - float, int ....
    int sonda = hallRead(); //nacitanie udajov z hallovej sondy, systemova funkcia
    String hall = String(sonda); //prevedenie na String do pomocnej funkcie
    unsigned long beh = millis();
    String uptime = String(beh);
  if (client.connect(host, 443)) { //ak je pripojenie uspesne
    Serial.println("Pripojenie pre odoslanie dat uspesne");
    String url = "/esp32/zapisdata.php?teplota="+temp+"&hall="+hall+"&temp2="+temp2+"&hum="+hum+"&uptime="+uptime+"&signal="+rssi+"&ssid="+ssid; //vyskladany HTTP request s dvomi hodnotami, oddelene ampersandom & - pre rozlisenie na strane webu
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
  }else{ //ak je pripojenie neuspesne, informuj do UART monitoru
      Serial.println("Nepodarilo sa odoslat data"); //informativny vypis
    } 
    client.stop();
}

void loop() {
  if (WiFi.waitForConnectResult() == WL_CONNECTED) { //ak je pripojenie uspesne
    pocitadlo = 0; //vynuluj pocitadlo
    Serial.println("Wifi je stale pripojene s IP: "); 
    Serial.println(WiFi.localIP());   //informuj pouzivatela o jeho IP v kazdej slucke
    rssi = WiFi.RSSI();
    Serial.print("Sila signalu (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  }else if (WiFi.waitForConnectResult() != WL_CONNECTED) { //ak nie je uspesne, opakuj spojenie
    Serial.println("Spojenie stratene - Pripajanie k wifi..."); 
    WiFi.begin(ssid, pass);      
    digitalWrite(led, LOW);
  }
  while (WiFi.status() != WL_CONNECTED) { //pozdrz program, pokym sa doska pripoji
    delay(500);
    Serial.print(".");
    pocitadlo++;
    if(pocitadlo>=60){ //po 30 sekundach restartuj dosku softverovym resetom, ak nedojde k pripojeniu
    ESP.restart();
    }
  }
  digitalWrite(led, HIGH);
  odoslanie(); //volanie funkcie pre odosielanie
  stav_rele(); //volanie funkcie pre citanie stavu rele
  restart(); //volanie funkcie pre citanie vyziadania resetu
  delay(200); //pockaj 2 sekundy pred novou sluckou
}
