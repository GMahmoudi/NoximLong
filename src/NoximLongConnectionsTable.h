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
 * This file contains the implementation of the long connections table
 */

#ifndef __NOXIMLONGCONNECTIONS_TABLE_H__
#define __NOXIMLONGCONNECTIONS_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "NoximMain.h"
using namespace std;

// Structure used to store information into the table
struct NoximLongConnection {
  int node_1;			// ID of the first node
  int node_2;			// ID of the desond node
};

class NoximLongConnectionsTable {

  public:

    NoximLongConnectionsTable();

    // Load long connections table from file. Returns true if ok, false otherwise
    bool load(const char *fname);
    //returns true if the node has a long connection.
    bool hasLongConnection(const int id);
    //returns the ID of the pair node (i.e. the node connected with through a long connection)
    	//if the given node ID has no long connection the function return -1.
    int getIDofPairNode(const int id);
    bool isItNodeOneOfTheLongConnection(const int id);
    bool isItNodeTwoOfTheLongConnection(const int id);
    int  getPositionInFiveDirTilesArray(const int id);

  private:
     vector < NoximLongConnection > long_connections_table;
};

#endif
