#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <fstream>

using namespace std;

int main()
{
    guide::TransportCatalogue catalogue;
    map_renderer::MapRenderer map_renderer;
    //guide::FormTransportBaseAndRequests(cin, catalogue, map_renderer, cout);
    /*std::ofstream out; // поток для записи
    out.open("out.txt");
    std::ifstream in("case_input_2.txt");*/
    setlocale(LC_ALL, "Russian");
    guide::FormTransportBaseAndRequests(cin, catalogue, map_renderer, cout);
    //catalogue.GetAllInfo();
}
