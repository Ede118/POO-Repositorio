#include <Arduino.h>

/**
* @file    poo_genera_rand.ino
* @version  1.1
* @date   2023.07.11
* @author Cesar Omar Aranda
* @contact  cesar.aranda@ingenieria.uncuyo.edu.ar
* @description
*   El monitor serie se encuentra a la escucha, al recibir un caracter 'x', 'j' o 'c',
*   devuelve un registro con valores al azar, generados con semilla variable, 
*   para los parametros dispositivo_id, porcentaje_valvula, estado_nivel y caudal,
*   usando formatos XML, Json y CSV (separado por comas)
**/

int disp_id = 2; // identificador dado a este dispositivo
long porc_aper; // aleatorio para simular porcentaje apertura valvula u otro
String nivel;
String niveles[] = {"bajo", "medio", "alto"}; // posibles valores
float caudal;
char option = ' '; // opcion recibida
String sret; // cadena devuelta


// Obtener valores para cada variable
void get_random(){
  //Genera un numero aleatorio entero entre 1 y 100
  porc_aper = random(1,100);

  //Genera un numero aleatorio real entre 1 y 100
  caudal = float(random(1,10000))/100;

  //Genera un aleatorio para seleccionar un estado del nivel
  nivel = niveles[random(1,4)];
}

// devuelve un documento XML con valores en las variables establecidas
String xml_response(){
  String resp = "<?xml version = \"1.0\" ?>\n";
  resp += "<registro>\n";
  resp += " <dispositivo_id>";
  resp += String (disp_id);
  resp += "</dispositivo_id>\n";
  resp += "  <porcentaje_valvula>";
  resp += String (porc_aper);
  resp += "</porcentaje_valvula>\n";
  resp += "  <estado_nivel>";
  resp += String (nivel);
  resp += "</estado_nivel>\n";
  resp += "  <caudal>";
  resp += String (caudal);
  resp += "</caudal>\n";
  resp += "</registro>\n";
  return resp;
}

// devuelve un documento Json con valores en las variables establecidas
String json_response(){
  String resp = "{\n";
  resp += "  \"dispositivo_id\": ";
  resp += String (disp_id) + ",\n";
  resp += "  \"porcentaje_valvula\": ";
  resp += String (porc_aper) + ",\n";
  resp += "  \"estado_nivel\": ";
  resp += "\"" + String (nivel) + "\",\n";
  resp += "  \"caudal\": ";
  resp += String (caudal) + ",\n";
  resp += "}\n";
  return resp;
}

// devuelve un documento CSV con valores en las variables establecidas
String csv_response(){
  String resp = "";
  resp += String (disp_id);
  resp += ",";
  resp += String (porc_aper);
  resp += ",";
  resp += String (nivel);
  resp += ",";
  resp += String (caudal);
  resp += "\n";
  return resp;
}

//Función de inicialización
void setup(){
  //Inicializamos la comunicación serial
  Serial.begin(9600);
  
  //Escribimos por el puerto serie mensaje de inicio
  Serial.println("Inicio de sketch: escucha/genera numero aleatorio");
  Serial.println("x: cadena XML");
  Serial.println("j: cadena Json");
  Serial.println("c: cadena CSV");
      
  //Establecemos la semilla en un pin analogico 
  randomSeed(analogRead(A0));  
}

void loop() {
  if(Serial.available() != 0){
    option = Serial.read();
    if(option == 'x' || option == 'j' || option == 'c' ){
      get_random();

      switch(option){
        case 'x':
        sret = xml_response();
          break;
        case 'j':
        sret = json_response();
          break;
        case 'c':
        sret = csv_response();
          break;
      }
      //Colocamos los datos en el puerto serie
      Serial.println(sret);
    }
  }
  
  delay(100);
}