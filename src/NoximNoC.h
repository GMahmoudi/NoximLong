 /*
 *  #######################################################
 *  # NoximLong - the NoC with long connections Simulator #
 *  #######################################################
 *
 *  NoximLong - The NoC Simulator with support for long connections between the mesh nodes.
 *  NoximLong is developed by the University of Tishreen, Syria.
 *  2015
 *
 *  NoximLong is based on Noxim - the NoC Simulator developed by the University of Catania
 *  For the complete list of authors refer to file ../doc/AUTHORS.txt
 *  For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file represents the top-level testbench
 */

#ifndef __NOXIMNOC_H__
#define __NOXIMNOC_H__

#include <systemc.h>
#include "NoximTile.h"
#include "NoximTile_5Directions.h"
#include "NoximGlobalRoutingTable.h"
#include "NoximGlobalTrafficTable.h"
#include "NoximLongConnectionsTable.h"

using namespace std;

SC_MODULE(NoximNoC)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the NoC
    sc_in < bool > reset;	// The reset signal for the NoC

    // Signals
    sc_signal <bool> req_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    sc_signal <bool> ack_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    sc_signal <NoximFlit> flit_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <NoximFlit> flit_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <NoximFlit> flit_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <NoximFlit> flit_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    sc_signal <int> free_slots_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    // NoP
    sc_signal <NoximNoP_data> NoP_data_to_east[MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_west[MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_south[MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_north[MAX_STATIC_DIM][MAX_STATIC_DIM];

    //Additional signals for 5Directions tiles
    sc_signal <bool> req_to_5thDirection[MAX_NUMBER_OF_FIVE_DIRECTIONAL_TILES];
    sc_signal <bool> ack_to_5thDirection[MAX_NUMBER_OF_FIVE_DIRECTIONAL_TILES];
    sc_signal <NoximFlit> flit_to_5thDirection[MAX_NUMBER_OF_FIVE_DIRECTIONAL_TILES];
    sc_signal <int> free_slots_to_5thDirection[MAX_NUMBER_OF_FIVE_DIRECTIONAL_TILES];
    sc_signal <NoximNoP_data> NoP_data_to_5thDirection[MAX_NUMBER_OF_FIVE_DIRECTIONAL_TILES];

    // Matrix of tiles
    NoximTile *t[MAX_STATIC_DIM][MAX_STATIC_DIM];
    NoximTile_5Directions *t_5directions[MAX_NUMBER_OF_FIVE_DIRECTIONAL_TILES];

    // Global tables
    NoximGlobalRoutingTable grtable;
    NoximGlobalTrafficTable gttable;
	NoximLongConnectionsTable lctable;

    //---------- Mau experiment <start>
    void flitsMonitor() {

	if (!reset.read()) {
	    //      if ((int)sc_simulation_time() % 5)
	    //        return;
	    unsigned int count = 0;
	    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++)
		for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++)
		{
			NoximCoord coord;
			coord.x = i;
			coord.y = j;
			int node_id = coord2Id(coord);
			if (lctable.hasLongConnection(node_id) ) 
			{
			  count += t_5directions[lctable.getPositionInFiveDirTilesArray(node_id)]->r->getFlitsCount();
			}
			else count += t[i][j]->r->getFlitsCount();
		}
	    cout << count << endl;
	}
    }
    //---------- Mau experiment <stop>

    // Constructor

    SC_CTOR(NoximNoC) {

	//---------- Mau experiment <start>
	/*
	   SC_METHOD(flitsMonitor);
	   sensitive(reset);
	   sensitive_pos(clock);
	 */
	//---------- Mau experiment <stop>
	// Build the Mesh
	buildMesh();
    }

    int XY2Id(const int i, const int j) const;  
    NoximTile *searchNode(const int id) const;
   
    bool hasLongConnection(int id);
    int getIDofPairNode(const int id);

    NoximTile_5Directions *getAFiveDirectionalNode(const int id);

  private:
    void buildMesh();

};

#endif
