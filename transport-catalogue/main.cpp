#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <clocale>
#include <fstream>

using namespace std;

int main()
{
    guide::TransportCatalogue catalogue;
    map_renderer::MapRenderer map_renderer;
    router::TransportRouter transport_router;
    guide::RequestHandler request_handler(catalogue, map_renderer, transport_router);
    // guide::FormTransportBaseAndRequests(cin, catalogue, map_renderer, cout);
    // std::ofstream out; // поток для записи
    // out.open("out.txt");
    // std::ifstream in("in.txt");
    //setlocale(LC_NUMERIC, "");
    guide::FormTransportBaseAndRequests(cin, catalogue, map_renderer, transport_router,request_handler, cout);
    // out.close();
    //  transport_router.GetBusInfo();
    //  catalogue.GetAllInfo();
}
