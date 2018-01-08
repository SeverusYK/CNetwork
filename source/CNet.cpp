#include<iostream>
#include<cstdlib>
#include<vector>
#include<random>
#include<algorithm>
#include<fstream>
#include<string>
#include<sstream>
#include<map>

#include "SparseMatrix.cpp"

using namespace std;

/* =======================================================================================================

Victor Buendía's CNetwork Class

This software is Open Source and redistributed with a MIT LICENSE (Read LICENSE file for more details).
Please follow license terms when using this code.

========================================================================================================== */

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================


/** \brief CNetwork base class
*  \author Victor Buendia
*
*   CNetwork is the core class for a weighted, undirected network.
*   Needs two template arguments: class associated to nodes and links
*
*/
template <class T = bool, class B = bool>
class CNetwork
{
    public:

        /** \brief Add new nodes to the network
        *  \param n: number of nodes to add
        *
        * Add new nodes to the network
        */
        void add_nodes(int n);


        /** \brief Remove a node from the network
        *  \param index: index of the node to remove
        *
        * Remove the selected node from the network. All links related to this node will be erased.
        */
        bool remove_node(int index);


        /** \brief Add a link to the network
        *  \param from: index of the origin node
        *  \param to: index of the target node
        *
        * Adds a link from the nodes from and to
        */
        void add_link(int from, int to);

        /** \brief Add a link to the network
        *  \param from: index of the origin node
        *  \param to: index of the target node
        *  \param w: weight of the link.
        *
        * Add a weighted link between nodes from and to
        */
        void add_link(int from, int to, B w);

        /** \brief Remove a link from the network
        *  \param from: Index of the origin node
        *  \param to: Index of the target node
        *  \return false if there is no link between from and to.
        *
        * Remove the a link between nodes from and to, if it exist.
        */
        bool remove_link(int from, int to);


        /** \brief Compute mean degree of the network
        *  \return Mean degree
        *
        * Computes the mean degree of the network
        */
        double mean_degree();

        /** \brief Computes the clustering coefficient of a node
        *  \param index: target node
        *  \return clustering coefficient of target node
        *
        * Compute a clustering coefficient of a target node.
        */
        double clustering_coef(int node_index);

        /** \brief Computes verage clustering coefficient
        *  \return clustering coefficient of the network
        *
        * Computes the clustering coefficient of each element, and takes the average.
        * It is not the same as the one computed counting triangles.
        */
        double mean_clustering_coef();

        /** \brief Performs a BFS from the selected node
        *  \param node: index of the target node
        *  \param[out] node_indices: contains unsorted indices of the nodes in network
        *  \param[out] dist: element j is the distance to node_indices[j]
        *
        *  Computes a BFS using the algorithm given in Newman's book. Returns the distance to all
        *  the nodes in the network from the target node. Distances are unsorted, so an auxiliar
        *  vector is provided. dist[j] is distance to node_indices[j]
        */
        void breadth_first_search(int node, vector<int> &node_indices, vector<int> &dist);

        /** \brief Computes the nodes in the same component as target
        *  \param index: target node
        *  \param[out] list_nodes: indices of nodes in the same component as target
        *  \param comp_size: optional. If component size is known before hand, it speeds up the method.
        *
        * Use BFS to see which nodes are in the same network component than me. If the size of the component
        * is known beforehand, it can be given to the algorithm to make it faster.
        */
        void component_nodes(int index, vector<int> &list_nodes, int comp_size = -1);

        /** \brief Finds all not-connected components of network and computes the sizes
        *  \param[out] node_in_this_component: each component j is represented by a node index j.
        *  \param[out] size_of_components: element j of this list contains size of component j
        *
        * Use BFS to find all network components. Each component, j, is represented by its size, and by
        * a node that is inside the component. This node can be used to recover the full component with
        * component_nodes, if needed.
        */
        void component_size(vector<int> &node_in_this_component, vector<int> &size_of_components);

        /** \brief Computes the component with largest size, and return it
        *  \return size of the largest component
        *
        * Use BFS to compute component of network with largest size
        */
        int largest_component_size();

        /** \brief Computes the degree distribution of the network
        *  \param[out] distribution: index j contains number of nodes with degree j. It is the degree distribution
        *  \param normalized: optional. If set to true, returns a normalized distribution. False by default.
        *
        * Compute the degree distribution of the network. If you also need the correlations, please use instead degree_correlation.
        */
        void degree_distribution(vector<int> &distribution, bool normalized = false);

        /** \brief Computes the degree distribution and correlations
        *  \param[out] distribution: index j contains number of nodes with degree j. It is the degree distribution
        *  \param[out] correlation: index j contains average degree of neighbours of a node with degree j
        *  \param normalized: optional. If set to true, returns a normalized distribution. False by default.
        *
        * Computes the average number of neighbours that a node with degree j has. It also computes and stores the degree distribution,
        * since both quantities are usually needed.
        */
        void degree_correlation(vector<int> &distribution, vector<double> &correlation, bool normalized = false);


        /** \brief Computes the average pathlenght of the network
        *  \return pathlenght of the network.
        *
        * Uses BFS to compute the average pathlenght of the complete network (even if it is disconnected). To compute
        * pathlenght of single network component, use average_pathlenght_component instead
        */
        double average_pathlenght();


        /** \brief Computes the average pathlenght of a component
        *  \param cmponent_index: index of a node that belongs to the desired component
        *  \param comp_size: optional. Size of the desired component
        *
        * Uses BFS to compute the average pathlenght of a component, that is selected via the index of a node that belongs
        * to the component. If size of the component is known beforehand, it speeds up the computations
        */
        double average_pathlenght_component(int component_index, int comp_size = -1);


        /** \brief Generates an Albert-Barabasi network
        *  \param n: nodes of the network
        *  \param m0: initial number of fully-connected nodes
        *  \param m: new links added for each node
        *  \param random_seed: optional, default 123456789. Same seed gives the same network.
        *
        * Generates an Albert-Barabasi network based in the algorithm given by Newman. The random seed should be
        * specified for obtaining different networks each iteration.
        */
        void create_albert_barabasi(int n, int m0, int m, unsigned int random_seed = 123456789);

        /** \brief Generates a scale-free network using the configurational model
        *  \param nodes: nodes of the network
        *  \param kmin: minimum degree of each node
        *  \param gamma: exponent of power law
        *  \param random_seed: optional, default 123456789. Same seed gives the same network.
        *
        * Generates a scale free network based using the configuration model. The random seed should be
        * specified for obtaining different networks each iteration.
        */
        void create_configurational(int nodes, int kmin, double gamma, unsigned int random_seed);


        /** \brief Generates a Watts-Strogatz network
        *  \param nodes: nodes of the network
        *  \param regular_connections: degree of each node in the case p=0
        *  \param p: probability of shuffling edges
        *  \param random_seed: optional, default 123456789. Same seed gives the same network.
        *
        * Generates a Watts-Strogatz network. In the case of a completely regular network (p=0), each node
        * has regular_connections edges. The random seed should be specified for obtaining different networks each iteration.
        */
        void create_watts_strogatz(int nodes, int regular_connections, double p, unsigned int random_seed);


        /** \brief Generates a random Erdos-Renyi network
        *  \param nodes: nodes of the network
        *  \param mean_k: average degree
        *  \param random_seed: optional, default 123456789. Same seed gives the same network.
        *
        * Generates an Erdos-Renyi network. The random seed should be specified for obtaining different networks each iteration.
        */
        void create_erdos_renyi(int nodes, double mean_k, unsigned int random_seed);


        /** \brief Gets the degree of the target node
        *  \param node_index: target node
        *  \return degree of target node
        *
        * Returns the degree of the target node
        */
        int degree(int node_index);


        /** \brief Gets a link by its index
        *  \param link_index: index of target link
        *  \return vector containing origin and target node indices
        *
        * Returns a vector (node_origin, node_destination) given the link index
        */
        vector<int> get_link(int link_index);


        /** \brief Gets a link weight
        *  \param link_index: index of target link
        *  \return link weight
        *
        * Returns the object ("weight") associated with the specified link
        */
        B get_weight(int link_index);

        /** \brief Gets a link between two specified nodes
        *  \param from: origin node
        *  \param to: destination node
        *  \return index of the link between from and destination.
        *
        * Returns the index of the link that connects nodes from and to. If there is no link, then
        * it returns -1.
        */
        int get_link_index(int from, int to);


        /** \brief Total number of nodes in the network
        *  \return number of nodes in the network
        *
        * Returns the total number of nodes initialized in the network
        */
        int get_node_count();


        /** \brief Total number of links in the network
        *  \return number of links in the network
        *
        * Returns the total number of links initialized in the network
        */
        int get_link_count();


        /** \brief Get the neighbours of a given node
        *  \param node_index: target node
        *  \return vector containing the indices of the neighbour nodes of the target node
        *
        * Returns the a vector with the indices of the neighbours of the specified node.
        */
        vector<unsigned int> get_neighs(int node_index);


        /** \brief Selects a neighbour of a given node
        *  \param node_index: target node
        *  \param k: neighbour to be selected
        *  \return index of the k-th neighbour of the target node
        *
        * Returns the index of the k-th neighbour of the target node. Neighbours are unsorted
        */
        int get_neigh_at(int node_index, int k);


        /** \brief Define a new tag for the network
        *  \param name: tag identifier
        *  \param type: can be int, double, string, or bool.
        *  \param is_for_nodes: if false, this property is assigned to links
        *
        * Define a new property or tag. This will be exported to GraphML. It can
        * be used also as additional properties for network dynamics.
        */
        void define_property(string name, string type, bool is_for_nodes);


        /** \brief Set the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \param value: the new value
        *
        * Set a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        void set_value(string name, int index, double value);


        /** \brief Set the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \param value: the new value
        *
        * Set a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        void set_value(string name, int index,  int value);


        /** \brief Set the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \param value: the new value
        *
        * Set a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        void set_value(string name, int index,  bool value);


        /** \brief Set the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \param value: the new value
        *
        * Set a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        void set_value(string name, int index,  string value);



        /** \brief Get the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \return value stored
        *
        * Get a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        double get_value_d(string name, int index);


        /** \brief Get the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \return value stored
        *
        * Get a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        int get_value_i(string name, int index);



        /** \brief Get the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \return value stored
        *
        * Get a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        bool get_value_b(string name, int index);



        /** \brief Get the value of an existing property
        *  \param name: tag identifier
        *  \param index: index or node or link where it has to be stored
        *  \return value stored
        *
        * Get a value for the property "name" in the node or link index. Note that
        * user must take care of manually checking index bounds.
        */
        string get_value_s(string name, int index);



        /** \brief Export network data to GraphML, Gephi compatible format
        *  \param filename: name of the file (without extension)
        *  \param labels: optional. Tags used to name the nodes
        *
        * Export the network data. Properties defined via define_property will be stored as node or link
        * properties. In addition to that, it is also possible to label directly the nodes. This will be
        * recognized as a node identifier in software like Gephi. For compatibility, MTX format is preferred
        */
        void write_graphml(string filename, vector<string> labels = vector<string>());



        /** \brief Export network data to plain MTX format
        *  \param filename: name of the file (without extension)
        *
        * The MTX format is defined by a simple plaintext representation of the adjancency matrix.
        * This is compatible with most network-analysis software, and it is easy to read from any language.
        */
        void write_mtx(string filename);



        /** \brief Read network data from plain MTX format
        *  \param filename: name of the file (including extension)
        *
        * The MTX format is defined by a simple plaintext representation of the adjancency matrix.
        * This function reads any MTX-like format
        */
        void read_mtx(string filename);


        /** \brief Assignement operator
        *  \param other: another CNetwork of the same type
        *  \return CNetwork
        *
        * Assigns a network to another.
        */
        CNetwork<T,B> &operator=(const CNetwork<T,B> &other);


        /** \brief Bracket operator
        *  \param i: index
        *  \return reference to the value stored in i-th node
        *
        * Access the value stored in the i-th node.
        */
        T& operator[](const int& i);



        /** \brief CNetwork standard constructor
        *  \param max_size: maximum size of the network
        *
        * Creates a new CNetwork with a limit to the number of nodes. The maximum is fixed and
        * the memory for the nodes is not allocated.
        */
        CNetwork(int max_size);



        /** \brief Largest eigenvalue calculator
        *  \param approx_error: desired margin of error for the eigenvalue
        *  \param max_it: optional. Limits the number of iterations. Default: 20
        *  \return vector that contains the largest eigenvalue (last element) and corresponding eigenvector.
        *
        * Computes the largest eigenvalue of the adjacency matrix using a power method. It returns a vector that
        * has the largest eigenvalue as the last element. The other values are the eigenvector.
        */
        vector<double> compute_eigenv(double approx_error, int max_it = 20);



        /** \brief Delete all the data stored by the network
        *  \param max_size: maximum size of the network
        *
        * Delete everything stored by the network.
        */
        void clear_network();


        SparseMatrix<B> adjm;


    private:


        int max_net_size;
        int current_size;
        int link_count;

        vector< vector<unsigned int> > neighs;

        vector<T> value;

        map<string, vector<double> > prop_d;
        map<string, vector<int> > prop_i;
        map<string, vector<bool> > prop_b;
        map<string, vector<string> > prop_s;





};

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================



template <class T, typename B>
CNetwork<T,B>::CNetwork(int max_size)
{
    max_net_size = max_size; //Set the max size of the network
    clear_network(); //Initialize everything
    return;
}

template <class T, typename B>
void CNetwork<T,B>::clear_network()
{
    current_size = 0; //Init size to 0
    link_count = 0; //Init link count
    //Create vectors in order to add things
    adjm = SparseMatrix<B>(max_net_size, true);

    neighs = vector< vector<unsigned int> >(0, vector<unsigned int>(0));


    prop_d = map<string, vector<double> >();
    prop_i = map<string, vector<int> >();
    prop_b = map<string, vector<bool> >();
    prop_s = map<string, vector<string> >();

    value = vector<T>();


    return;
}

template <class T, typename B>
CNetwork<T,B> &CNetwork<T,B>::operator= (const CNetwork<T,B> &other)
{
    adjm = other.adjm;

    max_net_size = other.max_net_size;
    current_size = other.current_size;
    link_count = other.link_count;

    value = other.value;
    prop_d = other.prop_d;
    prop_b = other.prop_b;
    prop_i = other.prop_i;
    prop_s = other.prop_s;

    return (*this);
}

template <class T, typename B>
T& CNetwork<T,B>::operator[](const int& i)
{
    return value[i];
}

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================


template <class T, typename B>
void CNetwork<T,B>::add_nodes(int n)
{
    int old_size = current_size; //Store old size
    int next_size = old_size + n; //New size of the network

    //Set the size to the maximum if it exceeds it
    current_size = next_size > max_net_size ? max_net_size : next_size;

    value.resize(current_size); //Increase the number of value.size without adding any element to it

    for (int i = old_size; i < current_size; i++)
    {
        neighs.push_back(vector<unsigned int>()); //Add a new container for neighbours
    }
    return;
}

template <class T, typename B>
bool CNetwork<T,B>::remove_node(int index)
{
    int i,j,k;

    if (index >= 0 and index < current_size)
    {
        neighs.erase(neighs.begin() + index); //Delete its entry in the neighbours
        value.erase(value.begin() + index); //Delete its value

        //Re-scale all the neighbours index to fit the net, and also erase
        //the entry of the removed node as neighbour of others
        for (i=0; i < neighs.size(); i++)
        {
            for (j=0; j < degree(i); j++)
            {
                k = get_neigh_at(i,j);
                if (k == index) //If it is the one, I want to erase, remove
                {
                    neighs[i].erase(neighs[i].begin() + j);
                }
                else if (k > index) //If it is higher, move 1 to left
                {
                    neighs[i][j] -= 1;
                }
            }
        }

        vector<int> who_to_erase = vector<int>(); //Track who you want to delete

        for (i=0; i < link_count; i++)
        {
            //Separe cases from-to in order to correctly reduce degree
            if (adjm[i].x == index || adjm[i].y == index)
            {
                //Our node is detected, erase the entry to that node
                who_to_erase.push_back(i);
            }

            //Also reduce index of nodes higher than our
            if (adjm[i].x > index)
            {
                //links[2*i] -= 1;
                adjm[i].x -= 1;
            }
            if (adjm[i].y > index)
            {
                //links[2*i+1] -= 1;
                adjm[i].y -= 1;
            }
        }

        //Perform the erase process
        for (i=0; i < who_to_erase.size(); i++)
        {
            adjm.erase(who_to_erase[i]); //Delete all links
            //Index was recorded before, we do 2*who in order to get as even-odd again
        }

        link_count -= who_to_erase.size() / 2; //Reduce the number of links
        current_size -= 1; //Reduce the current size of the network in one unit

        return true;

    }
    else return false;
}

template <class T, typename B>
void CNetwork<T,B>::add_link(int from, int to)
{
    adjm.push_back(data<bool>(from, to, true)); //Assume this method is for bools

    neighs[from].push_back(to); //Add the node to the neighbours
    neighs[to].push_back(from); //And do it in the other sense also

    link_count += 1; //Create one link more
    return;
}

template <class T, typename B>
void CNetwork<T,B>::add_link(int from, int to, B w)
{
    adjm.push_back(data<B>(from, to, w)); //Assume this method is for weighted things

    neighs[from].push_back(to); //Add the node to the neighbours
    neighs[to].push_back(from); //And do it in the other sense also

    link_count += 1; //Create one link more

    return;
}

template <class T, typename B>
bool CNetwork<T,B>::remove_link(int from, int to)
{
    //Reduce the degree of the nodes
    auto index_it = find(neighs[from].begin(), neighs[from].end(), to); //Relative index of TO in terms of FROM
    int index_neigh = distance(neighs[from].begin(), index_it); //Get the relative index as an int


    if (index_neigh >= 0 and index_neigh < neighs[from].size())
    {
        //int index_neigh = distance(neighs[from].begin(), index_it); //Get the relative index as an int
        int index_link = get_link_index(from, to);

        adjm.erase(index_link); //Delete the link

        link_count -= 1;


        neighs[from].erase(neighs[from].begin()+index_neigh); //Erase the node it pointed in the neighbours list

        //Do the same process, but now in the other node, the to one.
        //Since this node was in the neigh list of FROM, we know that FROM has to be in the list of TO
        //That's why we don't check again if index_it < neighs end
        index_it = find(neighs[to].begin(), neighs[to].end(), from);
        index_neigh = distance(neighs[to].begin(), index_it);

        neighs[to].erase(neighs[to].begin()+index_neigh);

        return true;
    }
    else return false;

}


// ========================================================================================================
// ========================================================================================================
// ========================================================================================================



template <class T, typename B>
double CNetwork<T,B>::mean_degree()
{
    int i;
    double sum = 0.0; //Get the sum,

    //Sum over the network
    for (i=0; i < current_size; i++)
    {
        sum += degree(i);
    }
    //Divide by current size
    return sum / (current_size * 1.0);
}

template <class T, typename B>
double CNetwork<T,B>::clustering_coef(int node_index)
{
    if (degree(node_index) > 1) //If we have more than one neighbour...
    {
        int i,j;
        int counter; //Count of pairs


        vector<unsigned int> nodes_neigh = get_neighs(node_index);
        vector<unsigned int> nodes_check;

        counter = 0;
        for (i=0; i < nodes_neigh.size(); i++) //Get neighbours of our node
        {
            nodes_check = get_neighs(nodes_neigh[i]); //Get neighbours of node i

            //For the next nodes, (start in j=i+1 to avoid double-count a pair)
            for (j=i+1; j < nodes_neigh.size(); j++)
            {
                //Use the  find function to see if this node is connected to any other neighbours of our node.
                //In that case, increase the counter
                if (find(nodes_check.begin(), nodes_check.end(), nodes_neigh[j]) != nodes_check.end()) counter += 1;
            }
        }

        return 2.0 * counter / (degree(node_index) * (degree(node_index) - 1)); //Finish computation and return clustering coefficient
    }
    else //... in other case, we cannot have common neighbours
    {
        return 0.0;
    }

}


template <class T, typename B>
double CNetwork<T,B>::mean_clustering_coef()
{
    int i;
    double sum = 0.0; //Get the sum,

    //Sum over the network
    for (i=0; i < current_size; i++)
    {
        sum += clustering_coef(i);
    }
    //Divide by current size
    return sum / (current_size * 1.0);
}


template <class T, typename B>
void CNetwork<T,B>::breadth_first_search(int node, vector<int> &node_indices, vector<int> &dist)
{
    int w, r; //Write and read pointers;
    int d; //Current distance
    int i,j; //Counter
    int cur_node; //Current node we are evaluating
    int neigh; //Current neighbour we are seeing

    //Init everything
    for (i=0; i < current_size; i++)
    {
        node_indices[i] = -1;
        dist[i] = -1;
    }

    //Get first node
    node_indices[0] = node;
    dist[node] = 0;
    //Quantities,
    r = 0; //Reading first element,
    w = 1; //Writing second

    while(w != r)
    {
        cur_node = node_indices[r]; //The node we want to evaluate now
        r += 1; //We will want to take the next
        d = dist[cur_node]; //Get the distance this node is at,
        for (j=0; j < degree(cur_node); j++)
        {
            neigh = get_neigh_at(cur_node, j); //Get the neighbour
            if (dist[neigh] == -1) //If distance is unknown,
            {
                dist[neigh] = d+1; //Put its distance,
                node_indices[w] = neigh; //Add it into the list of known nodes
                w += 1; //and increase write pointer
            }
        }
    }

    return;
}


template <class T, typename B>
void CNetwork<T,B>::component_nodes(int index, vector<int> &list_nodes, int comp_size)
{
    int i,j;


    vector<int> node_list(current_size); //List of nodes
    vector<int> dist(current_size); //Distance to nodes

    //If user has specified component-size, allocate memory once and do all the stuff...
    if (comp_size > 0)
    {
        list_nodes = vector<int>(comp_size);

        breadth_first_search(index, node_list, dist); //Use this to get all the nodes in my component

        i = 0; //Counter for every node
        j = 0;
        //j is the number of nodes in the component. If we reach the known number comp_size, stop looking for more
        while (j < comp_size)
        {
            if (dist[i] >= 0)
            {
                list_nodes[j] = i;
                j++;
            }
            i++;
        }
        /*for (i = 0; i < current_size; i++)
        {
            if (dist[i] >= 0)
            {
                list_nodes[j] = i;
                j++;
            }
        }*/
    }
    //If not, declare vectors and push_back the elements.
    else
    {
        list_nodes = vector<int>();

        breadth_first_search(index, node_list, dist); //Use this to get all the nodes in my component

        j = 0;
        for (i = 0; i < current_size; i++)
        {
            if (dist[i] >= 0)
            {
                list_nodes.push_back(i);
                j++;
            }
        }
    }


    return;
}


template <class T, typename B>
void CNetwork<T,B>::component_size(vector<int> &node_in_this_component, vector<int> &size_of_components)
{
    int i,j,k;

    int remaining = current_size; //How many nodes we have to evaluate yet

    node_in_this_component = vector<int>();
    size_of_components = vector<int>();

    vector<bool> visited(current_size, false); //List of visited nodes
    vector<int> node_list(current_size); //List of nodes
    vector<int> dist(current_size); //Distance to nodes
    int result;

    i = 0; //Init counters
    k = 0;
    while (i < current_size and remaining > 0) //While we have work to do,
    {
        if (!visited[i]) //If we have not visited this node before,
        {
            remaining -= 1; //We have one less to see
            visited[i] = true; //Mark it
            breadth_first_search(i, node_list, dist); //Compute paths from i to all other nodes and store them

            size_of_components.push_back(0); //Size is equal to zero when we start
            node_in_this_component.push_back(i); //The node we are now visiting is in this component - store it

            //Check every node
            for (j=0; j < current_size; j++)
            {
                if (dist[j] >= 0) //If distance is positive, this is inside my cluster
                {
                    size_of_components[k] += 1;
                    visited[j] = true;
                    remaining -= 1;
                }
            }

            k += 1; //Increase the counter for size of components

        }

        i += 1;
    }


    return;
}

template <class T, typename B>
int CNetwork<T,B>::largest_component_size()
{
    int largest_size = 0;
    int i;

    //Vectors for computing component size
    vector<int> node_in_this_component;
    vector<int> size_of_components;

    component_size(node_in_this_component, size_of_components); //Get the size of all components

    return *max_element(size_of_components.begin(), size_of_components.end());
}


template <class T, typename B>
double CNetwork<T,B>::average_pathlenght()
{
    int i,j; //Counters

    int counter = 0;
    double pathlenght; //To account for pathlenght

    vector<int> node_list(current_size); //List of nodes
    vector<int> dist(current_size); //Distance to nodes

    pathlenght = 0.0; //Start sum for average
    counter = 0;
    for (i=0; i < current_size; i++)
    {
        breadth_first_search(i, node_list, dist); //Get the distance to all the other nodes

        //If the distance is greater than 0, then add it to average
        for (j=0; j < current_size; j++)
        {
            if (dist[j] > 0)
            {
                pathlenght += dist[j];
                counter++;
            }
        }
    }

    return (counter > 0) ? pathlenght / (1.0 * counter) : -1.0; //Return the average pathlenght of the network
}


template <class T, typename B>
double CNetwork<T,B>::average_pathlenght_component(int component_index, int comp_size)
{
    int i,j;


    int counter;
    double pathlength;

    int index;

    vector<int> cluster_index; //What nodes are reachable from me
    vector<int> node_list(comp_size); //List of nodes
    vector<int> dist(current_size); //Distance to nodes

    //Get the nodes in this component
    component_nodes(component_index, cluster_index, comp_size);

    counter = 0;
    pathlength = 0.0;
    for (i=0; i < cluster_index.size(); i++)
    {
        index = cluster_index[i];
        breadth_first_search(index, node_list, dist); //Now compute distances
        for (j=0; j < cluster_index.size(); j++)
        {
            if (cluster_index[j] != index)
            {
                pathlength += dist[cluster_index[j]];
                counter++;
            }
        }
    }


    return (counter > 0) ? pathlength / (1.0 * counter) : -1.0; //Return the average pathlenght of the network
}



template <class T, typename B>
void CNetwork<T,B>::degree_distribution(vector<int> &distribution, bool normalized)
{
    int i;
    distribution = vector<int>(current_size, 0);
    //Compute the degree distribution
    for (i=0; i < current_size; i++)
    {
        distribution[degree(i)] += 1;
    }

    //Erase the 0s at the end of the array.
    i = current_size - 1; //Start counter
    while (distribution[i] == 0)
    {
        //distribution.erase(distribution.begin() + i);
        distribution.pop_back();
        i -= 1;
    }

    //Normalize the distribution if it has been indicated
    if (normalized)
    {
        for (i=0; i < distribution.size(); i++)
        {
            distribution[i] /= link_count;
        }
    }
    return;
}

template <class T, typename B>
void CNetwork<T,B>::degree_correlation(vector<int> &distribution, vector<double> &correlation, bool normalized)
{
    int i,j,k;
    int index, numneighs;
    double mean_neigh_degree; //Average degree of the neighbour of a node

    int maxdegree = 0;
    //Get the maximum degree of the network
    for (i=0; i < current_size; i++)
    {
        if (degree(i) > maxdegree)
        {
            maxdegree = degree(i);
        }
    }

    //Use it to create containers able to handle the distribution
    correlation = vector<double>(maxdegree, 0.0);
    distribution = vector<int>(maxdegree, 0);

    //Now compute the distributions
    for (i=0; i < current_size; i++)
    {
        index = degree(i); //Get the index of the vector of distribution
        distribution[index] += 1; //And set that it has one count more

        numneighs = index; //num_neighs has to be equal to the degree
        mean_neigh_degree = 0.0;
        for (j=0; j < numneighs; j++)
        {
            k = get_neigh_at(i,j); //Get index of the neigh
            mean_neigh_degree += degree(k); //Add its degree
        }
        if (numneighs != 0) mean_neigh_degree /= 1.0 * numneighs; //Finish the average
        correlation[index] += mean_neigh_degree; //Put it into the distribution
    }

    //To finish average over nodes, divide by the number of nodes with degree k
    //This are in distribution. Check that we are not dividing by 0
    for (i=0; i < correlation.size(); i++)
    {
        if (distribution[i] != 0) correlation[i] /= 1.0*distribution[i];
    }

    if (normalized)
    {
        for (i=0; i < distribution.size(); i++)
        {
            distribution[i] /= link_count;
        }
    }

    return;
}

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================

template <class T, typename B>
void CNetwork<T,B>::create_erdos_renyi(int n, double mean_k, unsigned int random_seed)
{
    int i,j,k;
    double p = mean_k / (n - 1.0);

    mt19937 gen(random_seed);; //Create the generator
    uniform_real_distribution<double> ran_u(0.0,1.0); //Uniform number distribution
    uniform_int_distribution<int>  index(0,n-1); //-1 because closed interval for ints

    add_nodes(n); //Create the nodes

    //For the n (n-1) / 2 pairs, link them with probability p
    for (i=0; i < current_size; i++)
    {
        for (j=i+1; j < current_size; j++)
        {
            if (ran_u(gen) <= p)
            {
                add_link(i, j);
            }
        }
    }

    return;

}

template <class T, typename B>
void CNetwork<T,B>::create_configurational(int n, int mink, double gamma, unsigned int random_seed)
{
    int i,j;
    int n_links;
    int max_size; //Maximum size if we want an uncorrelated network

    vector<int> node_degree;
    vector<int> link_vector;

    mt19937 gen(random_seed); //Create the generator
    uniform_real_distribution<double> ran_u(0.0,1.0); //Uniform number distribution

    add_nodes(n); //Add the nodes we need
    node_degree = vector<int>(current_size); //Store degree of every node
    max_size = sqrt(current_size); //Max size to avoid correlatons

    n_links = 0; //There's no link yet

    //Compute all the quantities we need to generate the degrees,
    double kmax = pow(max_size, 1.0-gamma);
    double kmin = pow(mink, 1.0-gamma);
    double invgamma = 1.0 / (1.0 - gamma);


    for (i=0; i < current_size; i++)
    {
        node_degree[i] = floor( pow( ran_u(gen)*(kmax - kmin) + kmin, invgamma ) ); //Generate degrees
        n_links += node_degree[i]; //Update number of links
    }

    //Make sure we have even number of links
    if (n_links % 2 == 1)
    {
        node_degree[0] += 1;
        n_links += 1;
    }

    link_vector = vector<int>(n_links); //Initalize the vector which will create the links

    int k=0;
    for (i=0; i < current_size; i++)
    {
        //Put index i a number ki of times
        for (j=0; j < node_degree[i]; j++)
        {
            link_vector[k] = i;
            k += 1;
        }
    }

    random_shuffle(link_vector.begin(), link_vector.end()); //Make a shuffle

    //Now create links using shuffled pairs, and that's all
    for (i=0; i < link_vector.size()/2; i++)
    {
        if (link_vector[2*i] != link_vector[2*i+1])
        {
            add_link(link_vector[2*i], link_vector[2*i+1]);
        }
    }

    return;

}

template <class T, typename B>
void CNetwork<T,B>::create_watts_strogatz(int n, int num_forward_edges, double p, unsigned int random_seed)
{
    int i,j;
    int to;
    mt19937 gen(random_seed);; //Create the generator
    uniform_real_distribution<double> ran_u(0.0,1.0); //Uniform number distribution
    uniform_int_distribution<int>  index(0,n-1); //-1 because closed interval for ints
    vector<unsigned int> aux;
    bool eliminated;


    clear_network();
    ///TODO check for reg_connections = 0, warn the user

    //Add the nodes
    add_nodes(n);

    for (i=0; i < current_size; i++)
    {
        for (j=1; j <= num_forward_edges; j++) //At least one node.
        {
            if (ran_u(gen) > p) add_link(i, (i+j)%current_size); //Add the link
            else
            {
                do
                {
                    to = index(gen); //Get a new neighbour
                    aux = get_neighs(i);
                }
                //Do it again if I selected exactly the same node, or if it is myself
                while (to == i && find(aux.begin(), aux.end(), to) != aux.end());
                add_link(i, to);
            }
        }
    }

    return;
}


template <class T, typename B>
void CNetwork<T,B>::create_albert_barabasi(int n, int m0, int m, unsigned int random_seed)
{
    int i,j,k,l;
    double r;

    mt19937 gen(random_seed);; //Create the generator
    uniform_real_distribution<double> random(0.0,1.0); //Uniform number distribution
    uniform_int_distribution<int>  index(0,1); //Useful to select indices

    int index_add;
    vector<int> yet_linked(m-1, -1); //Stores with which nodes I have visited. I only need to remember m-1 -I don't have to store the last
    bool found;

    ///TODO: solve the yet_linked problem

    //Create fully connected network with m0 nodes
    add_nodes(m0);
    for (i=0; i < m0; i++)
    {
        for (j=i+1; j < m0; j++)
        {
            add_link(i,j);
        }
    }

    //Add then more nodes...
    for (i = m0; i < n; i++)
    {
        add_nodes(1);

        yet_linked = vector<int>(m-1, -1);
        k = 0;
        //For every link we want to do,
        for (j=0; j < m; j++)
        {
            //Generate a random number
            if (random(gen) <= 0.5)
            {
                //With half probability, add it to highly connected node, selecting randomly an edge.
                index = uniform_int_distribution<int>(0, link_count-1);//-1 because interval is closed
                index_add = adjm[index(gen)].y;

            }
            else
            {
                //If not, connect to a random node
                index = uniform_int_distribution<int>(0, current_size-2); //We don't want current_size-1 which is the actual node, so up to -2
                index_add = index(gen);
            }

            //Check there that we don't have still a link between the two...
            found = false;
            l = 0;
            while (l < m-1 && !found && yet_linked[l] != -1) //Remember yet_linked.size = m - 1 always
            {
                found = index_add == yet_linked[l];
                l++;
            }
            //If there is no previous link between both, then add it.
            if (!found)
            {
                add_link(current_size-1, index_add); //Add the link
                yet_linked[k] = index_add; //Say we explored it
                k++; //To fill next vector element
            }

        }
    }

    return;
}

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================

// ========================================================================================================
// ========================================================================================================
// ========================================================================================================

template <class T, typename B>
int CNetwork<T,B>::degree(int node_index)
{
    return neighs[node_index].size();
}


template <class T, typename B>
int CNetwork<T,B>::get_link_index(int from, int to)
{
    int i,even,odd;
    bool found = false;
    i = 0;

    while (i < link_count and not found)
    {
        found = (adjm[i].x == from and adjm[i].y == to) or (adjm[i].x == to and adjm[i].y == from);
        i += 1;
    }

    return found ? i-1 : -1; //Remember we have just summed i
}

template <class T, typename B>
vector<int> CNetwork<T,B>::get_link(int link_index)
{
    return {adjm[link_index].x, adjm[link_index].y};
}

template <class T, typename B>
B CNetwork<T,B>::get_weight(int link_index)
{
    return adjm[link_index].value;
}

template <class T, typename B>
int CNetwork<T,B>::get_node_count()
{
    return current_size;
}

template <class T, typename B>
int CNetwork<T,B>::get_link_count()
{
    return link_count;
}

template <class T, typename B>
vector<unsigned int> CNetwork<T,B>::get_neighs(int node_index)
{
    return neighs[node_index];
}

template <class T, typename B>
int CNetwork<T,B>::get_neigh_at(int node_index, int k)
{
    return neighs[node_index][k];
}

template <class T, typename B>
void CNetwork<T,B>::define_property(string name, string type, bool is_for_nodes)
{
    int n = is_for_nodes ? current_size : link_count;

    if (type == "double")
    {
        prop_d[name] = vector<double>(n, 0.0);
    }
    else if (type == "int")
    {
        prop_i[name] = vector<int>(n, 0);
    }
    else if (type == "bool")
    {
        prop_b[name] = vector<bool>(n, false);
    }
    else if (type == "string")
    {
        prop_s[name] = vector<string>(n, "");
    }
    return;
}

template <class T, typename B>
void CNetwork<T,B>::set_value(string name, int index, double value)
{
    prop_d[name][index] = value;
}
template <class T, typename B>
void CNetwork<T,B>::set_value(string name, int index, int value)
{
    prop_i[name][index] = value;
}
template <class T, typename B>
void CNetwork<T,B>::set_value(string name, int index, bool value)
{
    prop_b[name][index] = value;
}
template <class T, typename B>
void CNetwork<T,B>::set_value(string name, int index, string value)
{
    prop_s[name][index] = value;
}

template <class T, typename B>
double CNetwork<T,B>::get_value_d(string name, int index)
{
    return prop_d[name][index];
}
template <class T, typename B>
int CNetwork<T,B>::get_value_i(string name, int index)
{
    return prop_i[name][index];
}
template <class T, typename B>
bool CNetwork<T,B>::get_value_b(string name, int index)
{
    return prop_b[name][index];
}
template <class T, typename B>
string CNetwork<T,B>::get_value_s(string name, int index)
{
    return prop_s[name][index];
}


template <class T, typename B>
void CNetwork<T,B>::write_graphml(string filename, vector<string> labels)
{
    int i,j,k;
    ofstream output;
    string is_for_nodes;

    output.open(filename + ".graphml"); //Open the file


    //Write the XML header
    output << "<?xml version='1.0' encoding='utf-8'?>" << endl; //Encoding
    //All the URL info for graphml format
    output << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" ";
    output << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns ";
    output << "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\"> " << endl;
    //Define the weight as an attribute
    output << "<key attr.name=\"weight\" attr.type=\"double\" for=\"edge\" id=\"w\" />" << endl;

    //Create all the properties
    for (auto &property : prop_d)
    {
        is_for_nodes = property.second.size() == current_size ? "node" : "edge";
        output << "<key attr.name=\"" << property.first << "\" attr.type=\"double\" for=\""<< is_for_nodes <<"\" id=\"id_" << property.first << "\" />" << endl;
    }
    for (auto &property : prop_i)
    {
        is_for_nodes = property.second.size() == current_size ? "node" : "edge";
        output << "<key attr.name=\"" << property.first << "\" attr.type=\"int\" for=\""<< is_for_nodes <<"\" id=\"id_" << property.first << "\" />" << endl;
    }
    for (auto &property : prop_b)
    {
        is_for_nodes = property.second.size() == current_size ? "node" : "edge";
        output << "<key attr.name=\"" << property.first << "\" attr.type=\"boolean\" for=\""<< is_for_nodes <<"\" id=\"id_" << property.first << "\" />" << endl;
    }
    for (auto &property  : prop_s)
    {
        is_for_nodes = property.second.size() == current_size ? "node" : "edge";
        output << "<key attr.name=\"" << property.first << "\" attr.type=\"string\" for=\""<< is_for_nodes <<"\" id=\"id_" << property.first << "\" />" << endl;
    }



    //Create graph
    output << "<graph edgedefault=\"undirected\">" << endl;

    //If we have labels
    if (labels.size() > 0)
    {

        for (i=0; i < current_size; i++)
        {
            output << "<node id=\"" << labels[i] << "\">" << endl; //Then define nodes as labels
            //Property addition
            for (auto &property : prop_d)
            {
                if (property.second.size() == current_size ) //If this property if for nodes,
                {
                    //Then add it to our node
                    output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }

            }
            //Do the same with all other properties
            for (auto &property : prop_i)
            {
                if (property.second.size() == current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_b)
            {
                if (property.second.size() == current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_s)
            {
                if (property.second.size() == current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            output << "</node>" << endl;
        }

        for (i=0; i < link_count; i++)
        {
            //And link using the labels
            output << "<edge source=\"" << labels[adjm[i].x] << "\" target=\"" << labels[adjm[i].y] << "\">" << endl;
            for (auto &property : prop_d)
            {
                if (property.second.size() != current_size ) //If this property if for links,
                {
                    //Then add it to our node
                    output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }

            }
            //Do the same with all other properties
            for (auto &property : prop_i)
            {
                if (property.second.size() != current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_b)
            {
                if (property.second.size() != current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_s)
            {
                if (property.second.size() != current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            output << "</edge>" << endl;
        }
    }
    else
    {
        for (i=0; i < current_size; i++)
        {
            output << "<node id=\"" << i << "\">" << endl; //Then define nodes as labels
            //Property addition
            for (auto &property : prop_d)
            {
                if (property.second.size() == current_size ) //If this property if for nodes,
                {
                    //Then add it to our node
                    output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }

            }
            //Do the same with all other properties
            for (auto &property : prop_i)
            {
                if (property.second.size() == current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_b)
            {
                if (property.second.size() == current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_s)
            {
                if (property.second.size() == current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            output << "</node>" << endl;
        }

        for (i=0; i < link_count; i++)
        {
            //And link using the labels
            output << "<edge source=\"" << adjm[i].x << "\" target=\"" << adjm[i].y << "\">" << endl;
            for (auto &property : prop_d)
            {
                if (property.second.size() != current_size ) //If this property if for links,
                {
                    //Then add it to our node
                    output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }

            }
            //Do the same with all other properties
            for (auto &property : prop_i)
            {
                if (property.second.size() != current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_b)
            {
                if (property.second.size() != current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            for (auto &property : prop_s)
            {
                if (property.second.size() != current_size )
                {
                   output << "<data key=\"id_" << property.first << "\">"  << property.second[i] << "</data>" << endl;
                }
            }
            output << "</edge>" << endl;
        }
    }


    //Close the open tags and the file
    output << "</graph>" << endl << "</graphml>" << endl;
    output.close();
}

template <class T, typename B>
void CNetwork<T,B>::write_mtx(string filename)
{
    ofstream output;
    int i;

    output.open(filename + ".mtx");
    //Write that this is a NxN matrix with link_count links
    output << "%Network created using CNetwork 1.0" << endl;
    output << current_size << " " << current_size << " " << link_count << endl;
    //Check if the used decided to use a weighted net
    if (typeid(B) == typeid(bool))
    {
        for (i=0; i < link_count; i++)
        {
            output << adjm[i].x << " " << adjm[i].y << endl; //Using a weight 1
        }
    }
    else
    {
        for (i=0; i < link_count; i++)
        {
            output << adjm[i].x << " " << adjm[i].y << " " << adjm[i].value << endl; //Arbitrary weight
        }
    }
    output.close();
}

template <class T, typename B>
void CNetwork<T,B>::read_mtx(string filename)
{
    //Destroy this object and create new network
    clear_network(max_net_size);

    bool read_header = false; //To see if we have read the dim1xdim2 links line
    string line; //Store a line
    ifstream input; //File
    int from, to, w; //Auxiliary

    //Open the file and checks avaiable
    input.open(filename);
    if (input.is_open())
    {
        while(getline(input, line)) //File we have not finished,
        {
            if (line[0] != '%') //If first char is not a comment
            {
                istringstream iss(line); //Transform into stream
                if (read_header) //Have we read the header?
                {
                    //Then check if network is weighted
                    if (typeid(B) == typeid(bool))
                    {
                        iss >> from >> to; //Fill vars with data from stream
                        add_link(from, to);
                    }
                    else
                    {
                        iss >> from >> to >> w; //Fill vars with data from stream
                        add_link(from, to, w);
                    }

                }
                else
                {
                    read_header = true; //Then mark header as read
                    iss >> from >> from >> to; //Matrix NxN so first two numbers are the same. "From" is N and "To" is the number of links
                    add_nodes(from); //Add N nodes to the network
                }

            }

        }
    }
    input.close();
}

template <class T, typename B>
vector<double> CNetwork<T,B>::compute_eigenv(double approx_error, int max_it)
{
    return adjm.dom_eigen(approx_error, max_it);
}

