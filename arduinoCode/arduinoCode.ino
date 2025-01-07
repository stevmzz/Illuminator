class Componente {
  protected:
    int pin;       // Número de pin del componente
    bool estado;   // Estado del componente (encendido/apagado)

  public:
    // Constructor
    Componente(int numeroPin) {
      pin = numeroPin;
      estado = false;
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

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar un LED
    }

    void escribir(int valor) override {
      // Implementación para encender/apagar el LED
    }
};

// Clase Sensor
class Sensor : public Componente {
  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar un sensor
    }

    int leer() override {
      // Implementación para leer valores del sensor
      return 0;
    }
};

// Clase Pantalla
class Pantalla : public Componente {
  public:
    // Constructor
    Pantalla(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar una pantalla
    }

    void escribir(int valor) override {
      // Implementación para escribir valores en la pantalla
    }
};

// Clase Botón
class Boton : public Componente {
  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar un botón
    }

    int leer() override {
      // Implementación para leer el estado del botón
      return 0;
    }
};
