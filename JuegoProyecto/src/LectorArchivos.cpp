#include "LectorArchivos.h"

//Funcion auxiliar para eliminar espacios en blanco al inicio y final de una cadena
static std::string limpiarEspacios(const std::string& s) {
    auto start = std::find_if_not(s.begin(), s.end(), ::isspace);
    if (start == s.end()) return ""; // cadena solo con espacios

    auto end = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    return std::string(start, end);
}

// Función auxiliar para analizar una línea de nodo
static bool analizarLineaNodo(const std::string& linea, NodoInfo& nodo) {
    std::stringstream ss(linea);
    char coma;

    // Formato: id, tipo, valor, x, y
    if (ss >> nodo.id >> coma
           >> nodo.tipo >> coma
           >> nodo.valor >> coma
           >> nodo.x >> coma
           >> nodo.y) {
        return true;
    }

    return false;
}

// Función auxiliar para analizar una línea de arista
static bool analizarLineaArista(const std::string& linea, AristaInfo& arista) {
    std::stringstream ss(linea);
    char coma;

    // Formato: origen, destino, peso
    if (ss >> arista.idOrigen >> coma
           >> arista.idDestino >> coma
           >> arista.peso) {
        return true;
    }

    return false;
}

bool leerNodos(
    const std::string& rutaArchivo,
    std::vector<NodoInfo>& nodos,
    std::vector<AristaInfo>& aristas
) {
    nodos.clear();
    aristas.clear();

    std::ifstream in(rutaArchivo);
    if (!in.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << rutaArchivo << "\n";
        return false;
    }

    std::string linea;
    bool enNodos = false;
    bool enConexiones = false; 

    while (std::getline(in, linea)) {
        std::string limpia = limpiarEspacios(linea);

        // Saltar líneas vacías
        if (limpia.empty()) {
            continue;
        }

        // Detectar secciones
        if (limpia.find("NODOS") != std::string::npos) {
            enNodos = true;
            enConexiones = false;
            continue; // la siguiente línea ya es la primera de nodos
        }

        if (limpia.find("CONEXIONES") != std::string::npos) {
            enConexiones = true;
            enNodos = false;
            continue; // la siguiente línea ya es la primera de conexiones
        }

        // Si estamos en la sección de nodos, parseamos nodos
        if (enNodos) {
            NodoInfo nodo;
            if (analizarLineaNodo(limpia, nodo)) {
                nodos.push_back(nodo);
            } else {
                std::cerr << "[ADVERTENCIA] Línea de nodo mal formada: " 
                          << limpia << "\n";
            }
            continue;
        }

        // Si estamos en la sección de conexiones, parseamos aristas
        if (enConexiones) {
            AristaInfo arista;
            if (analizarLineaArista(limpia, arista)) {
                aristas.push_back(arista);
            } else {
                std::cerr << "[ADVERTENCIA] Línea de conexión mal formada: " 
                          << limpia << "\n";
            }
            continue;
        }
    }

    if (nodos.empty()) {
        std::cerr << "[ERROR] No se cargó ningún nodo desde el archivo.\n";
        return false;
    }

    if (aristas.empty()) {
        std::cerr << "[ERROR] No se cargó ninguna conexión desde el archivo.\n";
        return false;
    }

    return true;
}