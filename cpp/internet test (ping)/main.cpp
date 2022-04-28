#include <iostream>
#include <cstring>

using namespace std;

int main(void)
{
    string dest_ip = "8.8.8.8"; // ping target
    int ping_count = 10;
    int last_live_delay = 5; // 5 min delay between ping
    int last_dead_delay = 1; // 1 min delay between ping
    char last_live = 'F'; // default last dead connection
    int average_ping_time;
    int percentage_ping_fail;


}