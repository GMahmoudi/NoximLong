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
 * This file contains the implementation of the Network-on-Chip
 */

#include "NoximNoC.h"

void NoximNoC::buildMesh()
{
// Check for long connections table availability
    if (NoximGlobalParams::long_connections_status == Long_Connections_Activated)
	assert(lctable.load(NoximGlobalParams::long_connections_filename));

    // Check for routing table availability
    if (NoximGlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
	assert(grtable.load(NoximGlobalParams::routing_table_filename));

    // Check for traffic table availability
    if (NoximGlobalParams::traffic_distribution == TRAFFIC_TABLE_BASED)
	assert(gttable.load(NoximGlobalParams::traffic_table_filename));

    // Create the mesh as a matrix of tiles
    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
	for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
	    // Create the single Tile with a proper name
		char tile_name[20];
	  	NoximCoord coord;
		coord.x = i;
		coord.y = j;
		int node_id = coord2Id(coord);
		int positionInFiveDirTilesArray = lctable.getPositionInFiveDirTilesArray(node_id);
		if (lctable.hasLongConnection(node_id))  
		{// it is a 5 directions tile!
			sprintf(tile_name, "5DirTile[%02d][%02d]", i, j);
			t_5directions[positionInFiveDirTilesArray] = new NoximTile_5Directions(tile_name);
			// Tell the router about its coordinates
			t_5directions[positionInFiveDirTilesArray]->r->configure(j * NoximGlobalParams::mesh_dim_x + i,
					  NoximGlobalParams::stats_warm_up_time,
					  NoximGlobalParams::buffer_depth,
					  grtable, this);
			// Tell the PE about its coordinates
			t_5directions[positionInFiveDirTilesArray]->pe->local_id = j * NoximGlobalParams::mesh_dim_x + i;
			t_5directions[positionInFiveDirTilesArray]->pe->traffic_table = &gttable;	// Needed to choose destination
			t_5directions[positionInFiveDirTilesArray]->pe->never_transmit = (gttable.occurrencesAsSource(t_5directions[positionInFiveDirTilesArray]->pe->local_id) == 0);

			// Map clock and reset
			t_5directions[positionInFiveDirTilesArray]->clock(clock);
			t_5directions[positionInFiveDirTilesArray]->reset(reset);

			// Map Rx signals
			t_5directions[positionInFiveDirTilesArray]->req_rx[DIRECTION_NORTH] (req_to_south[i][j]);
			t_5directions[positionInFiveDirTilesArray]->flit_rx[DIRECTION_NORTH] (flit_to_south[i][j]);
			t_5directions[positionInFiveDirTilesArray]->ack_rx[DIRECTION_NORTH] (ack_to_north[i][j]);
				    
			t_5directions[positionInFiveDirTilesArray]->req_rx[DIRECTION_EAST] (req_to_west[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->flit_rx[DIRECTION_EAST] (flit_to_west[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->ack_rx[DIRECTION_EAST] (ack_to_east[i + 1][j]);

			t_5directions[positionInFiveDirTilesArray]->req_rx[DIRECTION_SOUTH] (req_to_north[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->flit_rx[DIRECTION_SOUTH] (flit_to_north[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->ack_rx[DIRECTION_SOUTH] (ack_to_south[i][j + 1]);

			t_5directions[positionInFiveDirTilesArray]->req_rx[DIRECTION_WEST] (req_to_east[i][j]);
			t_5directions[positionInFiveDirTilesArray]->flit_rx[DIRECTION_WEST] (flit_to_east[i][j]);
			t_5directions[positionInFiveDirTilesArray]->ack_rx[DIRECTION_WEST] (ack_to_west[i][j]);
				
			// Map Rx signal for 5 directions tiles
			if ( lctable.isItNodeOneOfTheLongConnection(node_id))
			{
			t_5directions[positionInFiveDirTilesArray]->req_rx[4] (req_to_5thDirection[positionInFiveDirTilesArray +1]);
			t_5directions[positionInFiveDirTilesArray]->flit_rx[4] (flit_to_5thDirection[positionInFiveDirTilesArray +1]);
			t_5directions[positionInFiveDirTilesArray]->ack_rx[4] (ack_to_5thDirection[positionInFiveDirTilesArray +1]);
			}
			else
			{
			t_5directions[positionInFiveDirTilesArray]->req_rx[4] (req_to_5thDirection[positionInFiveDirTilesArray -1]);
			t_5directions[positionInFiveDirTilesArray]->flit_rx[4] (flit_to_5thDirection[positionInFiveDirTilesArray -1]);
			t_5directions[positionInFiveDirTilesArray]->ack_rx[4] (ack_to_5thDirection[positionInFiveDirTilesArray -1]);
			}

			// Map Tx signals
			t_5directions[positionInFiveDirTilesArray]->req_tx[DIRECTION_NORTH] (req_to_north[i][j]);
			t_5directions[positionInFiveDirTilesArray]->flit_tx[DIRECTION_NORTH] (flit_to_north[i][j]);
			t_5directions[positionInFiveDirTilesArray]->ack_tx[DIRECTION_NORTH] (ack_to_south[i][j]);

			t_5directions[positionInFiveDirTilesArray]->req_tx[DIRECTION_EAST] (req_to_east[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->flit_tx[DIRECTION_EAST] (flit_to_east[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->ack_tx[DIRECTION_EAST] (ack_to_west[i + 1][j]);

			t_5directions[positionInFiveDirTilesArray]->req_tx[DIRECTION_SOUTH] (req_to_south[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->flit_tx[DIRECTION_SOUTH] (flit_to_south[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->ack_tx[DIRECTION_SOUTH] (ack_to_north[i][j + 1]);

			t_5directions[positionInFiveDirTilesArray]->req_tx[DIRECTION_WEST] (req_to_west[i][j]);
			t_5directions[positionInFiveDirTilesArray]->flit_tx[DIRECTION_WEST] (flit_to_west[i][j]);
			t_5directions[positionInFiveDirTilesArray]->ack_tx[DIRECTION_WEST] (ack_to_east[i][j]);

			// Map Tx signal for 5 directions tiles
			t_5directions[positionInFiveDirTilesArray]->req_tx[4] (req_to_5thDirection[positionInFiveDirTilesArray]);
			t_5directions[positionInFiveDirTilesArray]->flit_tx[4] (flit_to_5thDirection[positionInFiveDirTilesArray]);
			t_5directions[positionInFiveDirTilesArray]->ack_tx[4] (ack_to_5thDirection[positionInFiveDirTilesArray]);

			// Map buffer level signals (analogy with req_tx/rx port mapping)
			t_5directions[positionInFiveDirTilesArray]->free_slots[DIRECTION_NORTH] (free_slots_to_north[i][j]);
			t_5directions[positionInFiveDirTilesArray]->free_slots[DIRECTION_EAST] (free_slots_to_east[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->free_slots[DIRECTION_SOUTH] (free_slots_to_south[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->free_slots[DIRECTION_WEST] (free_slots_to_west[i][j]);

			t_5directions[positionInFiveDirTilesArray]->free_slots_neighbor[DIRECTION_NORTH] (free_slots_to_south[i][j]);
			t_5directions[positionInFiveDirTilesArray]->free_slots_neighbor[DIRECTION_EAST] (free_slots_to_west[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->free_slots_neighbor[DIRECTION_SOUTH] (free_slots_to_north[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->free_slots_neighbor[DIRECTION_WEST] (free_slots_to_east[i][j]);

   			// Map buffer level signals (analogy with req_tx/rx port mapping) for 5 directions tiles
			t_5directions[positionInFiveDirTilesArray]->free_slots[4] (free_slots_to_5thDirection[positionInFiveDirTilesArray]);
			if (lctable.isItNodeOneOfTheLongConnection(node_id))
			{
				t_5directions[positionInFiveDirTilesArray]->free_slots_neighbor[4] (free_slots_to_5thDirection[positionInFiveDirTilesArray +1]);	    
			}
			else {
			t_5directions[positionInFiveDirTilesArray]->free_slots_neighbor[4] (free_slots_to_5thDirection[positionInFiveDirTilesArray-1]);	    
			}

			// NoP 
			t_5directions[positionInFiveDirTilesArray]->NoP_data_out[DIRECTION_NORTH] (NoP_data_to_north[i][j]);
			t_5directions[positionInFiveDirTilesArray]->NoP_data_out[DIRECTION_EAST] (NoP_data_to_east[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->NoP_data_out[DIRECTION_SOUTH] (NoP_data_to_south[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->NoP_data_out[DIRECTION_WEST] (NoP_data_to_west[i][j]);

			t_5directions[positionInFiveDirTilesArray]->NoP_data_in[DIRECTION_NORTH] (NoP_data_to_south[i][j]);
			t_5directions[positionInFiveDirTilesArray]->NoP_data_in[DIRECTION_EAST] (NoP_data_to_west[i + 1][j]);
			t_5directions[positionInFiveDirTilesArray]->NoP_data_in[DIRECTION_SOUTH] (NoP_data_to_north[i][j + 1]);
			t_5directions[positionInFiveDirTilesArray]->NoP_data_in[DIRECTION_WEST] (NoP_data_to_east[i][j]);

			// NoP for 5 direction tiles
			t_5directions[positionInFiveDirTilesArray]->NoP_data_out[4] (NoP_data_to_5thDirection[positionInFiveDirTilesArray]);
			if (lctable.isItNodeOneOfTheLongConnection(node_id))
				    t_5directions[positionInFiveDirTilesArray]->NoP_data_in[4] (NoP_data_to_5thDirection[positionInFiveDirTilesArray +1]);
			else
			        t_5directions[positionInFiveDirTilesArray]->NoP_data_in[4] (NoP_data_to_5thDirection[positionInFiveDirTilesArray -1]);
			
		}		
		else //it is a normal, 4 direction, tile
		{
			sprintf(tile_name, "Tile[%02d][%02d]", i, j);
		    t[i][j] = new NoximTile(tile_name);
		    // Tell to the router its coordinates
		    t[i][j]->r->configure(j * NoximGlobalParams::mesh_dim_x + i,
				  NoximGlobalParams::stats_warm_up_time,
				  NoximGlobalParams::buffer_depth,
				  grtable, this);
		    // Tell to the PE its coordinates
		    t[i][j]->pe->local_id = j * NoximGlobalParams::mesh_dim_x + i;
		    t[i][j]->pe->traffic_table = &gttable;	// Needed to choose destination
		    t[i][j]->pe->never_transmit = (gttable.occurrencesAsSource(t[i][j]->pe->local_id) == 0);
	
		    // Map clock and reset
		    t[i][j]->clock(clock);
		    t[i][j]->reset(reset);
	
		    // Map Rx signals
		    t[i][j]->req_rx[DIRECTION_NORTH] (req_to_south[i][j]);
		    t[i][j]->flit_rx[DIRECTION_NORTH] (flit_to_south[i][j]);
		    t[i][j]->ack_rx[DIRECTION_NORTH] (ack_to_north[i][j]);

		    t[i][j]->req_rx[DIRECTION_EAST] (req_to_west[i + 1][j]);
		    t[i][j]->flit_rx[DIRECTION_EAST] (flit_to_west[i + 1][j]);
		    t[i][j]->ack_rx[DIRECTION_EAST] (ack_to_east[i + 1][j]);
	
		    t[i][j]->req_rx[DIRECTION_SOUTH] (req_to_north[i][j + 1]);
		    t[i][j]->flit_rx[DIRECTION_SOUTH] (flit_to_north[i][j + 1]);
		    t[i][j]->ack_rx[DIRECTION_SOUTH] (ack_to_south[i][j + 1]);

		    t[i][j]->req_rx[DIRECTION_WEST] (req_to_east[i][j]);
		    t[i][j]->flit_rx[DIRECTION_WEST] (flit_to_east[i][j]);
		    t[i][j]->ack_rx[DIRECTION_WEST] (ack_to_west[i][j]);

		    // Map Tx signals
		    t[i][j]->req_tx[DIRECTION_NORTH] (req_to_north[i][j]);
		    t[i][j]->flit_tx[DIRECTION_NORTH] (flit_to_north[i][j]);
		    t[i][j]->ack_tx[DIRECTION_NORTH] (ack_to_south[i][j]);

		    t[i][j]->req_tx[DIRECTION_EAST] (req_to_east[i + 1][j]);
		    t[i][j]->flit_tx[DIRECTION_EAST] (flit_to_east[i + 1][j]);
		    t[i][j]->ack_tx[DIRECTION_EAST] (ack_to_west[i + 1][j]);

		    t[i][j]->req_tx[DIRECTION_SOUTH] (req_to_south[i][j + 1]);
		    t[i][j]->flit_tx[DIRECTION_SOUTH] (flit_to_south[i][j + 1]);
		    t[i][j]->ack_tx[DIRECTION_SOUTH] (ack_to_north[i][j + 1]);

		    t[i][j]->req_tx[DIRECTION_WEST] (req_to_west[i][j]);
		    t[i][j]->flit_tx[DIRECTION_WEST] (flit_to_west[i][j]);
		    t[i][j]->ack_tx[DIRECTION_WEST] (ack_to_east[i][j]);

		    // Map buffer level signals (analogy with req_tx/rx port mapping)
		    t[i][j]->free_slots[DIRECTION_NORTH] (free_slots_to_north[i][j]);
		    t[i][j]->free_slots[DIRECTION_EAST] (free_slots_to_east[i + 1][j]);
		    t[i][j]->free_slots[DIRECTION_SOUTH] (free_slots_to_south[i][j + 1]);
		    t[i][j]->free_slots[DIRECTION_WEST] (free_slots_to_west[i][j]);

		    t[i][j]->free_slots_neighbor[DIRECTION_NORTH] (free_slots_to_south[i][j]);
		    t[i][j]->free_slots_neighbor[DIRECTION_EAST] (free_slots_to_west[i + 1][j]);
		    t[i][j]->free_slots_neighbor[DIRECTION_SOUTH] (free_slots_to_north[i][j + 1]);
		    t[i][j]->free_slots_neighbor[DIRECTION_WEST] (free_slots_to_east[i][j]);

		    // NoP 
		    t[i][j]->NoP_data_out[DIRECTION_NORTH] (NoP_data_to_north[i][j]);
		    t[i][j]->NoP_data_out[DIRECTION_EAST] (NoP_data_to_east[i + 1][j]);
		    t[i][j]->NoP_data_out[DIRECTION_SOUTH] (NoP_data_to_south[i][j + 1]);
		    t[i][j]->NoP_data_out[DIRECTION_WEST] (NoP_data_to_west[i][j]);

		    t[i][j]->NoP_data_in[DIRECTION_NORTH] (NoP_data_to_south[i][j]);
		    t[i][j]->NoP_data_in[DIRECTION_EAST] (NoP_data_to_west[i + 1][j]);
		    t[i][j]->NoP_data_in[DIRECTION_SOUTH] (NoP_data_to_north[i][j + 1]);
		    t[i][j]->NoP_data_in[DIRECTION_WEST] (NoP_data_to_east[i][j]);
		}
	}
    }

    // dummy NoximNoP_data structure
    NoximNoP_data tmp_NoP;

    tmp_NoP.sender_id = NOT_VALID;

    for (int i = 0; i < DIRECTIONS_FIVE; i++) {
	tmp_NoP.channel_status_neighbor[i].free_slots = NOT_VALID;
	tmp_NoP.channel_status_neighbor[i].available = false;
    }
    // Clear signals for borderline nodes
    for (int i = 0; i <= NoximGlobalParams::mesh_dim_x; i++) {
	req_to_south[i][0] = 0;
	ack_to_north[i][0] = 0;
	req_to_north[i][NoximGlobalParams::mesh_dim_y] = 0;
	ack_to_south[i][NoximGlobalParams::mesh_dim_y] = 0;

	free_slots_to_south[i][0].write(NOT_VALID);
	free_slots_to_north[i][NoximGlobalParams::mesh_dim_y].write(NOT_VALID);

	NoP_data_to_south[i][0].write(tmp_NoP);
	NoP_data_to_north[i][NoximGlobalParams::mesh_dim_y].write(tmp_NoP);
    }

    for (int j = 0; j <= NoximGlobalParams::mesh_dim_y; j++) {
	req_to_east[0][j] = 0;
	ack_to_west[0][j] = 0;
	req_to_west[NoximGlobalParams::mesh_dim_x][j] = 0;
	ack_to_east[NoximGlobalParams::mesh_dim_x][j] = 0;

	free_slots_to_east[0][j].write(NOT_VALID);
	free_slots_to_west[NoximGlobalParams::mesh_dim_x][j].write(NOT_VALID);

	NoP_data_to_east[0][j].write(tmp_NoP);
	NoP_data_to_west[NoximGlobalParams::mesh_dim_x][j].write(tmp_NoP);

    }
    // invalidate reservation table entries for non-exhistent channels
    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
	if (!lctable.hasLongConnection(XY2Id(i, 0))) t[i][0]->r->reservation_table.invalidate(DIRECTION_NORTH);
    }
    for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
	if (!lctable.hasLongConnection(XY2Id(0, j))) t[0][j]->r->reservation_table.invalidate(DIRECTION_WEST);
	if (!lctable.hasLongConnection(XY2Id(NoximGlobalParams::mesh_dim_x - 1, j)))
		{ t[NoximGlobalParams::mesh_dim_x - 1][j]->r->reservation_table.invalidate(DIRECTION_EAST);}
    }
}

//Given the id, the following function returns the five directional tile...
NoximTile_5Directions *NoximNoC::getAFiveDirectionalNode(const int id)
{
   int positionIn5DirectionsArray = lctable.getPositionInFiveDirTilesArray(id);
   return t_5directions[positionIn5DirectionsArray];
}

int NoximNoC::XY2Id(const int i, const int j) const
{
    int id = (j * NoximGlobalParams::mesh_dim_x) + i;
    assert(id < NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);
    return id;
}

NoximTile *NoximNoC::searchNode(const int id) const
{
    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++)
	for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++)
	    if (t[i][j]->r->local_id == id)
		return t[i][j];
    return false;
}

bool NoximNoC::hasLongConnection(int id)
{
    return lctable.hasLongConnection(id);

}

int NoximNoC::getIDofPairNode(const int id)
{
	return lctable.getIDofPairNode(id);
}

