#include <iostream> 
#include <string> 
 
#include "input_reader.h" 
#include "stat_reader.h" 
 
using namespace std; 
 
int main() 
{ 
 
    guide::TransportCatalogue catalogue; 
 
    guide::FormTransportBase(cin, catalogue); 
 
    guide::GetRequests(catalogue, cin, cout); 
}