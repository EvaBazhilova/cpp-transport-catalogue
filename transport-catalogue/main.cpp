#include "json_reader.h"
#include "map_renderer.h"

#include <clocale>
#include <fstream>

using namespace std;

int main()
{
    guide::TransportCatalogue catalogue;
    map_renderer::MapRenderer map_renderer;
    guide::FormTransportBaseAndRequests(cin, catalogue, map_renderer, cout);
}
