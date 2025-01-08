#include <Adafruit_LiquidCrystal.h>

// Variables globales
bool modoAutomatico = true;  // Estado del modo automático

class Componente {
  protected:
    int pin; // Número de pin del componente
    bool estado; // Estado del componente

  public:
    // Constructor
    Componente(int numeroPin) {
      pin = numeroPin; // Asigna el número de pin recibido
      estado = false; // Inicialmente, el componente está apagado
    }
    // Métodos virtuales
    virtual void iniciar() {
      // Implementación base - puede ser sobrescrita por clases hijas
    }
    virtual void escribir(int valor) {
      // Implementación base - puede ser sobrescrita por clases hijas
    }
    virtual int leer() {
      // Implementación base - puede ser sobrescrita por clases hijas
      return 0;
    }
    // Métodos getter
    int obtenerPin() {
      return pin;
    }
    bool obtenerEstado() {
      return estado;
    }
    // Método setter
    void establecerEstado(bool nuevoEstado) {
      estado = nuevoEstado;
    }
};

// Clase LED
class LED : public Componente {
  public:
    // Constructor
    LED(int numeroPin) : Componente(numeroPin) {}
    
    void iniciar() override {
      pinMode(pin, OUTPUT); // Configura el pin como salida
      digitalWrite(pin, LOW); // Inicialmente apaga el LED
      estado = false; // Actualiza el estado interno
    }
    
    void escribir(int valor) override {
      digitalWrite(pin, valor); // Escribe el valor en el pin sea HIGH o LOW
      estado = (valor == HIGH); // Actualiza el estado
    }
};

// Clase Sensor
class Sensor : public Componente {
  private:
    int umbralLuz; // Umbral para la detección de luz

  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {
      umbralLuz = 5; // Valor predeterminado del umbral ajustado para condiciones de luz
    }

    void iniciar() override {
      pinMode(pin, INPUT); // Configura el pin como entrada
    }

    int leer() override {
      return analogRead(pin); // Lee el valor analógico del sensor
    }

    // Método para establecer el umbral de luz
    void establecerUmbralLuz(int nuevoUmbral) {
      umbralLuz = nuevoUmbral; 
    }

    // Método para obtener el umbral de luz
    int obtenerUmbralLuz() {
      return umbralLuz;
    }
};

// Clase Pantalla
class Pantalla : public Componente {
  private:
    Adafruit_LiquidCrystal lcd; // Instancia de la pantalla LCD
    int ledsEncendidos;         // Cantidad de LEDs encendidos

  public:
    // Constructor
    Pantalla(int direccionI2C) 
      : Componente(direccionI2C), lcd(direccionI2C, &Wire) {
        ledsEncendidos = 0; // Inicializar cantidad de LEDs encendidos
    }

    // Inicializar la pantalla
    void iniciar() override {
        lcd.begin(16, 2); // Configurar pantalla 16x2
        lcd.setBacklight(LOW); // Activar retroiluminación
        lcd.clear();
        lcd.print("Sistema iniciado");
    }

    // Actualizar la cantidad de LEDs encendidos
    void actualizarLedsEncendidos(int cantidad) {
        ledsEncendidos = cantidad;
    }

    // Mostrar información en la pantalla
    void mostrarInformacion(int valorLuz, int modo) {
        lcd.clear();
        lcd.setCursor(0, 0); // Primera línea
        lcd.print("Luz: ");
        lcd.print(valorLuz);
        
        lcd.setCursor(0, 1); // Segunda línea
        lcd.print("Modo: ");
        switch (modo) {
            case 0: lcd.print("Manual"); break;
            case 1: lcd.print("Fiesta"); break;
            case 2: lcd.print("Sueno"); break;
            case 3: lcd.print("Lectura"); break;
            case 4: lcd.print("Exterior"); break;
            default: lcd.print("Desconocido");
        }

        lcd.setCursor(10, 1);
        lcd.print("LEDs: ");
        lcd.print(ledsEncendidos);
    }
};

// Clase Botón
class Boton : public Componente {
  private:
    unsigned long ultimoTiempoBoton; // Para el control de rebote
    const long tiempoDebounce = 50; // Tiempo de debounce en ms
    int estadoAnterior; // Para detectar cambios
    bool esModoAutomatico; // Para diferenciar el tipo de botón

  public:
    // Constructor modificado para especificar el tipo de botón
    Boton(int numeroPin, bool modoAutomatico = false) : Componente(numeroPin) {
      ultimoTiempoBoton = 0;
      estadoAnterior = HIGH;
      esModoAutomatico = modoAutomatico;
    }

    void iniciar() override {
      pinMode(pin, INPUT_PULLUP);
      estadoAnterior = digitalRead(pin);
    }

    int leer() override {
      int estadoActual = digitalRead(pin);
      unsigned long tiempoActual = millis();

      if ((tiempoActual - ultimoTiempoBoton) > tiempoDebounce) {
        if (estadoActual == LOW && estadoAnterior == HIGH) {
          ultimoTiempoBoton = tiempoActual;
          
          if (esModoAutomatico) {
            estado = !estado; // Mantiene el estado para el botón de modo automático
          } else {
            return true; // Para botones manuales, solo retorna true cuando se presiona
          }
        }
      }
      
      estadoAnterior = estadoActual;
      return esModoAutomatico ? estado : false;
    }
};

// Variables para componentes
Sensor sensorLuz(A0);
LED ledsHabitaciones[] = {
    LED(2), // Sala
    LED(3), // Cocina
    LED(4), // Cuarto 1
    LED(5), // Cuarto 2
    LED(6), // Cuarto 3
    LED(7), // Patio interno
    LED(8)  // Patio frontal y trasero
};
Boton botonControl(9, true); // Botón para modo automático
Boton botonesManual[] = {
    Boton(10), // Sala
    Boton(11), // Cocina
    Boton(12), // Cuarto 1
    Boton(13), // Cuarto 2
    Boton(A1), // Cuarto 3
    Boton(A2), // Patio interno
    Boton(A3)  // Patio frontal y trasero
};
// Variables globales para el control de modos
int modoActual = 0; // 0: Sin modo, 1: Fiesta, 2: Sueño, 3: Lectura, 4: Exterior
Boton botonModo(A4); // Nuevo botón conectado al pin A4 para cambiar modos

// Función para manejar los modos de luces
void modosDeLuces() {
    // Verificar si el botón de modos fue presionado
    if (!botonModos.leer()) return;  // Salir si no se presiona el botón

    // Apagar todas las luces antes de cambiar de modo
    for (int i = 0; i < 7; i++) {
        ledsHabitaciones[i].escribir(LOW);
    }

    // Cambiar al siguiente modo
    modoActual = (modoActual + 1) % 5;  // Alternar entre 0 a 4 (5 modos)

    // Ejecutar acciones para el modo seleccionado
    switch (modoActual) {
        case 0:  // Apagado (salida para evitar interferencias)
            Serial.println("Modo Apagado: Todas las luces apagadas.");
            return;

        case 1:  // Modo Fiesta: Todos los LEDs parpadean
            Serial.println("Modo Fiesta: Todos los LEDs parpadean.");
            for (int i = 0; i < 5; i++) {  // Parpadeo rápido 5 veces
                for (int j = 0; j < 7; j++) {
                    ledsHabitaciones[j].escribir(HIGH);
                }
                delay(200);
                for (int j = 0; j < 7; j++) {
                    ledsHabitaciones[j].escribir(LOW);
                }
                delay(200);
            }
            break;

        case 2:  // Modo Sueño: Apagar todas las luces
            Serial.println("Modo Sueño: Todas las luces apagadas.");
            break;  // Ya están apagadas

        case 3:  // Modo Lectura: Encender solo la luz de la sala
            Serial.println("Modo Lectura: Luz de la sala encendida.");
            ledsHabitaciones[0].escribir(HIGH);  // Sala (LED en pin 2)
            break;

        case 4:  // Modo Exterior: Encender solo las luces de los patios
            Serial.println("Modo Exterior: Luces de los patios encendidas.");
            ledsHabitaciones[6].escribir(HIGH);  // Patio frontal y trasero (LED en pin 8)
            break;
    }
}

Pantalla pantalla(0x27);
// Configuración inicial del sistema
void setup() {
    Serial.begin(9600);
    
    // Inicializar pantalla LCD
    pantalla.iniciar();

    // Inicializar otros componentes
    sensorLuz.iniciar();
    sensorLuz.establecerUmbralLuz(5);

    botonControl.iniciar();
    botonModo.iniciar();

    for (int i = 0; i < 7; i++) {
        ledsHabitaciones[i].iniciar();
        botonesManual[i].iniciar();
    }
}

// Bucle principal del sistema
void loop() {
    // Verificar cambio en el modo automático
    if (botonControl.leer()) {
        modoAutomatico = !modoAutomatico;
        Serial.print("Modo Automático: ");
        Serial.println(modoAutomatico ? "Activado" : "Desactivado");
    }

    // Verificar si un botón manual fue presionado para salir de la función modosDeLuces
    if (modoActual != 0) {  // Si no estamos en modo normal (modoActual 0)
        modoActual = 0;  // Restablecer el modo a normal (salir de la función modosDeLuces)
        Serial.println("Modo de luces desactivado");
    }

    // Llamar a la función modosDeLuces solo si estamos en el modo normal
    if (modoActual == 0) {
        modosDeLuces();
    }
    int ledsEncendidos = 0;

    // Control automático
    if (modoAutomatico && modoActual == 0) { 
        int valorLuz = sensorLuz.leer();
        for (int i = 0; i < 7; i++) {
            bool ledEncendido = valorLuz > sensorLuz.obtenerUmbralLuz();
            ledsHabitaciones[i].escribir(ledEncendido ? HIGH : LOW);
            if (ledEncendido) ledsEncendidos++;
        }
        Serial.print("Sensor: ");
        Serial.print(valorLuz);
        Serial.print(" - LEDs: ");
        Serial.println(valorLuz > sensorLuz.obtenerUmbralLuz() ? 
                      "Encendidos" : "Apagados");
        pantalla.actualizarLedsEncendidos(ledsEncendidos);
        pantalla.mostrarInformacion(valorLuz, modoActual);
    }

    // Control manual
    if (!modoAutomatico && modoActual == 0) {
        for (int i = 0; i < 7; i++) {
            if (botonesManual[i].leer()) {
                bool estadoActual = ledsHabitaciones[i].obtenerEstado();
                ledsHabitaciones[i].escribir(estadoActual ? LOW : HIGH);
                if (!estadoActual) ledsEncendidos++;

                Serial.print("LED ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(!estadoActual ? "Encendido" : "Apagado");
            }
        }
        pantalla.actualizarLedsEncendidos(ledsEncendidos);
        pantalla.mostrarInformacion(sensorLuz.leer(), modoActual);
    }

    delay(100);
}