#include "Grafo.h"
#include <iostream>

void Grafo::construir(const std::vector<NodoInfo>& nodosInfo,
                      const std::vector<AristaInfo>& aristas)
{
    nodos.clear();
    for (const auto& n : nodosInfo) {
        Node nuevo;

        nuevo.id = n.id;
        nuevo.tipo = n.tipo;
        nuevo.valor = n.valor;
        nuevo.x = n.x;
        nuevo.y = n.y;

        nodos[n.id] = nuevo;  
    }

    for (const auto& a : aristas) {
        int u = a.idOrigen;   
        int v = a.idDestino;  
        int w = a.peso;

        if (nodos.find(u) == nodos.end() || nodos.find(v) == nodos.end()) {
            std::cerr << "Arista con nodo inexistente: "
                      << u << " - " << v << "\n";
            continue;
        }

        nodos[u].neighbors.push_back({v, w});
        nodos[v].neighbors.push_back({u, w});
    }

}














