// Definición de pines para el sensor y los LEDs
const int sensor = A0;                       // Sensor conectado a A0
const int leds[] = {2, 3, 4, 5, 6, 7, 8};    // Todos los LEDs
const int botonPin = 0;                      // Pin donde está el botón
const int umbralLuz = 500;                   // Umbral para encender o apagar los LEDs

// Variables para el control del modo automático
bool modoAutomatico = true;                  // Inicialmente en modo automático
int estadoAnteriorBoton = HIGH;              // Para detectar cambios en el botón
unsigned long ultimoTiempoBounsigned long ultimoTiempoBoton = 0;         // Último tiempo que se presionó el botón
ton = 0;         // Último tiempo que se presionó el botón
const long tiempoDebounce = 200;             // Tiempo de debounce en milisegundos

void setup() {
    // Configurar pines de los LEDs como salida
    for (int i = 0; i < 7; i++) {
        pinMode(leds[i], OUTPUT);
    }

    // Configurar el pin del sensor como entrada
    pinMode(sensor, INPUT);

    // Configurar el pin del botón como entrada con pull-up
    pinMode(botonPin, INPUT_PULLUP);

    Serial.begin(9600);  // Iniciar comunicación serial para monitoreo
}

void loop() {
    // Leer estado actual del botón
    int estadoBoton = digitalRead(botonPin);

    // Verificar si ha pasado suficiente tiempo desde la última pulsación
    if ((millis() - ultimoTiempoBoton) > tiempoDebounce) {
        // Si el botón se presiona (cambio de HIGH a LOW)
        if (estadoBoton == LOW && estadoAnteriorBoton == HIGH) {
            modoAutomatico = !modoAutomatico;  // Cambiar estado del modo automático
            Serial.print("Modo Automático: ");
            Serial.println(modoAutomatico ? "Activado" : "Desactivado");
            ultimoTiempoBoton = millis();  // Actualizar el tiempo de la última pulsación
        }
    }
    estadoAnteriorBoton = estadoBoton;

    // Solo procesar el sensor si el modo automático está activado
    if (modoAutomatico) {
        // Leer el valor del sensor
        int valorLuz = analogRead(sensor);

        // Encender o apagar todos los LEDs según el valor del sensor
        for (int i = 0; i < 7; i++) {
            digitalWrite(leds[i], valorLuz >= umbralLuz ? HIGH : LOW);
        }

        // Imprimir información de monitoreo
        Serial.print("Sensor: ");
        Serial.print(valorLuz);
        Serial.print(" - LEDs: ");
        Serial.println(valorLuz >= umbralLuz ? "Encendidos" : "Apagados");
    }

    delay(100);  // Pequeña pausa para estabilidad
}
