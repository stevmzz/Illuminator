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
    // Aquí puedes agregar variables específicas para la pantalla LCD
    
  public:
    // Constructor
    Pantalla(int numeroPin) : Componente(numeroPin) {}
    
    void iniciar() override {
      // Implementación para inicializar una pantalla
      // Agregar código específico para tu pantalla LCD
    }
    
    void escribir(int valor) override {
      // Implementación para escribir valores en la pantalla
      // Agregar código específico para tu pantalla LCD
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
    // Leer el botón de modo
    if (botonModo.leer()) {
        modoActual = (modoActual + 1) % 5; // Cambia al siguiente modo (0 a 4)
        
        // Apagar todos los LEDs para evitar confusión
        for (int i = 0; i < 7; i++) {
            ledsHabitaciones[i].escribir(LOW);
        }
    }

    // Activar el modo correspondiente
    switch (modoActual) {
        case 1: // Modo Fiesta
            for (int i = 0; i < 7; i++) {
                ledsHabitaciones[i].escribir(HIGH);
                delay(100);
                ledsHabitaciones[i].escribir(LOW);
            }
            break;
        case 2: // Modo Sueño
            // Todos los LEDs ya están apagados
            break;
        case 3: // Modo Lectura
            ledsHabitaciones[0].escribir(HIGH); // Encender solo la sala (LED 0)
            break;
        case 4: // Modo Exterior
            ledsHabitaciones[6].escribir(HIGH); // Encender patio interno (LED 6)
            ledsHabitaciones[5].escribir(HIGH); // Encender patio externo (LED 5)
            break;
    }
}


// Configuración inicial del sistema
void setup() {
    Serial.begin(9600);  // Iniciar comunicación serial para monitoreo

    // Inicializar sensor
    sensorLuz.iniciar();
    sensorLuz.establecerUmbralLuz(5);
    
    // Inicializar botón de modo
    botonControl.iniciar();
    botonModo.iniciar(); // Inicializar el botón para cambiar modos
    
    // Inicializar LEDs
    for (int i = 0; i < 7; i++) {
        ledsHabitaciones[i].iniciar();
    }
    
    // Inicializar botones de control manual
    for (int i = 0; i < 7; i++) {
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

    // Manejar los modos de luces
    modosDeLuces();

    // Control automático
    if (modoAutomatico && modoActual == 0) { // Solo funciona si no hay un modo activo
        int valorLuz = sensorLuz.leer();
        for (int i = 0; i < 7; i++) {
            ledsHabitaciones[i].escribir(
                valorLuz > sensorLuz.obtenerUmbralLuz() ? HIGH : LOW
            );
        }
        
        Serial.print("Sensor: ");
        Serial.print(valorLuz);
        Serial.print(" - LEDs: ");
        Serial.println(valorLuz > sensorLuz.obtenerUmbralLuz() ? 
                      "Encendidos" : "Apagados");
    }

    // Control manual
    if (!modoAutomatico && modoActual == 0) { // Solo funciona si no hay un modo activo
        for (int i = 0; i < 7; i++) {
            if (botonesManual[i].leer()) {
                bool estadoActual = ledsHabitaciones[i].obtenerEstado();
                ledsHabitaciones[i].escribir(estadoActual ? LOW : HIGH);
                
                Serial.print("LED ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(!estadoActual ? "Encendido" : "Apagado");
            }
        }
    }

    delay(100);  // Pequeña pausa para estabilidad
}
