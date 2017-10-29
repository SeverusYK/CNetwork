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

#define MAX_SIZE 1000000
#define IN 0
#define OUT 1

/* ==================================================================================

Victor Buend�a CNetwork Class

This software is Open Source and redistributed with a MIT LICENSE (Read LICENSE file for more details).
Please follow license terms when using this code.

=================================================================================== */

using namespace std;

/// ======================================================================================== ///
/// ==================================== Class definition ================================== ///
/// ======================================================================================== ///

template <class T = bool, typename B = bool>
class CNetwork
{
    public:

        void add_nodes(int n);
        bool remove_node(int index);
        void add_link(int from, int to);
        void add_link(int from, int to, B w);
        bool remove_link(int from, int to);
        void create_adjacency_matrix();

        double mean_degree();
        double clustering_coef(int node_index);
        double mean_clustering_coef();
        void breadth_first_search(int node, vector<int> &node_indices, vector<int> &dist);
        int component_size(vector<int> &node_in_this_component, vector<int> &size_of_components);
        vector<int> degree_distribution();
        vector<double> degree_correlation(vector<int> &distribution);
        double average_pathlenght();

        void create_albert_barabasi(int m0, int m, unsigned int random_seed);
        void create_configurational(int nodes, int kmin, double gamma, unsigned int random_seed);
        void create_wats_strogatz(int nodes, int regular_connections, double p, unsigned int random_seed);
        void create_erdos_renyi(int nodes, double mean_k, unsigned int random_seed);

        int degree(int node_index);
        vector<int> get_link(int link_index);
        B get_weight(int link_index);
        int get_link_index(int from, int to);
        int get_node_count();
        int get_link_count();
        vector<unsigned int> get_neighs(int node_index);
        int get_neigh_at(int node_index, int k);


        void define_property(string name, string type, bool is_for_nodes);
        void set_value(string name, int index, double value);
        void set_value(string name, int index,  int value);
        void set_value(string name, int index,  bool value);
        void set_value(string name, int index,  string value);
        double get_value_d(string name, int index);
        int get_value_i(string name, int index);
        bool get_value_b(string name, int index);
        string get_value_s(string name, int index);


        void write_graphml(string filename, vector<string> labels = vector<string>());
        void write_mtx(string filename);
        void read_mtx(string filename);

        void set_value(int index, T val);
        T get_value(int index);

        CNetwork(int max_size);

        vector<double> compute_eigenv(double approx_error, int max_it = 20);

        SparseMatrix<B> adjm;

    private:
        void clear_network(int max_size);

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

/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///


/// ======================================================================================== ///
/// ======================================== Constructor =================================== ///
/// ======================================================================================== ///

///Create a network with a maximum size max_size
template <class T, typename B>
CNetwork<T,B>::CNetwork(int max_size)
{
    clear_network(max_size);
    return;
}

template <class T, typename B>
void CNetwork<T,B>::clear_network(int max_size)
{
    current_size = 0; //Init size to 0
    max_net_size = max_size; //Set the max size of the network
    link_count = 0; //Init link count
    //Create vectors in order to add things
    adjm = SparseMatrix<B>(max_size, true);

    neighs = vector< vector<unsigned int> >(0, vector<unsigned int>(0));


    prop_d = map<string, vector<double> >();
    prop_i = map<string, vector<int> >();
    prop_b = map<string, vector<bool> >();
    prop_s = map<string, vector<string> >();

    value = vector<T>();


    return;
}



/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///

/// ======================================================================================== ///
/// ================================= Add info functions =================================== ///
/// ======================================================================================== ///


///Add n nodes to the network
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
            //links.erase(links.begin() + 2*who_to_erase[i] - 2*i); //-2*i to take in account how list moves
            //links.erase(links.begin() + 2*who_to_erase[i] - 2*i);  //This will eliminate the 2*i+1 since list moved to left
            //This uses directly the index
            //weight.erase(weight.begin() + who_to_erase[i] - i);
        }

        link_count -= who_to_erase.size() / 2; //Reduce the number of links
        current_size -= 1; //Reduce the current size of the network in one unit

        return true;

    }
    else return false;
}

///Create a link between nodes from and to and weight w
template <class T, typename B>
void CNetwork<T,B>::add_link(int from, int to)
{
    adjm.push_back(data<bool>(from, to, true)); //Assume this method is for bools
    //links.push_back(from); //Even to origin,
    //links.push_back(to); //Odd to destiny
    //weight.push_back(1.0); //Unit weight
    neighs[from].push_back(to); //Add the node to the neighbours
    neighs[to].push_back(from); //And do it in the other sense also
    link_count += 1; //Create one link more
    return;
}

///Create a link between nodes from and to and weight w
template <class T, typename B>
void CNetwork<T,B>::add_link(int from, int to, B w)
{

    /*int i = 0;
    bool existe = false;
    int rep_index;

    while (i < link_count and !existe)
    {
        if ((links[2*i] == from and links[2*i+1] == to) or (links[2*i] == to and links[2*i+1] == from))
        {
            rep_index = i;

            existe = true;
        }
        i+=1;
    }*/

    //if (!existe)
    //{

    adjm.push_back(data<B>(from, to, w)); //Assume this method is for weighted things

    //links.push_back(from); //Even to origin,
    //links.push_back(to); //Odd to destiny
    //weight.push_back(w); //Unit weight

    neighs[from].push_back(to); //Add the node to the neighbours
    neighs[to].push_back(from); //And do it in the other sense also

    link_count += 1; //Create one link more


    //}
    /*else
    {
        weight[rep_index] += w;
    }*/
    return;
}

///Remove a link between from and to
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

        adjm.erase(index_link);

        //weight.erase(weight.begin() + index_link); //Erase this link weight
        //links.erase(links.begin()+2*index_link);
        //links.erase(links.begin()+2*index_link);//Use this last index obtained to erase from link array

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

/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///

/// ======================================================================================== ///
/// ================================= Topology functions =================================== ///
/// ======================================================================================== ///

///Compute the mean degree of the network and returns it
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

///Computes the clustering coefficient of a particular node
template <class T, typename B>
double CNetwork<T,B>::clustering_coef(int node_index)
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
            //Use the  find function to see if this node is connected to any other neighbour of our node.
            //In that case, increase the counter
            if (find(nodes_check.begin(), nodes_check.end(), nodes_neigh[j]) != nodes_check.end()) counter += 1;
        }
    }

    if (degree(node_index) > 1)
    {
        return 2.0 * counter / (degree(node_index) * (degree(node_index) - 1)); //Finish computation and return clustering coefficient
    }
    else
    {
        return 0.0;
    }

}

///Computes the average clustering coefficient of the network
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


///Compute all the pathlenghts from node using the optimized version from Newman's book
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

///Use the breadth first search to get size of bigger component of the network
template <class T, typename B>
int CNetwork<T,B>::component_size(vector<int> &node_in_this_component, vector<int> &size_of_components)
{
    int i,j,k;

    int remaining = current_size;

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

    result = 0;
    for (i=0; i < size_of_components.size(); i++)
    {
        if (size_of_components[i] > result)
        {
            result = size_of_components[i];
        }
    }


    return result;
}

///Computes the average path lenght of the network
template <class T, typename B>
double CNetwork<T,B>::average_pathlenght()
{
    int i,j,k; //Counters
    //double sum;
    //int connected;
    int remaining = current_size; //How many nodes we have yet to evaluate
    int node; //auxiliar variable

    vector<bool> visited(current_size, false); //List of visited nodes
    vector<int> cluster_index = vector<int>(); //What nodes are reachable from me
    double pathlenght, maxpathlenght; //To account for pathlenght
    vector<int> node_list(current_size); //List of nodes
    vector<int> dist(current_size); //Distance to nodes

    maxpathlenght = 0.9; //Start with value > 0 so not every value will override this
    i = 0; //Init counters
    while (i < current_size and remaining > 0) //While we have work to do,
    {
        pathlenght = 0.0; //Start sum for average
        if (!visited[i]) //If we have not visited this node before,
        {
            remaining -= 1; //We have one less to see
            visited[i] = true; //Mark it
            breadth_first_search(i, node_list, dist); //Compute paths from i to all other nodes and store them

            //See which nodes are reachable from i
            for (j=0; j < current_size; j++)
            {
                if (dist[j] >= 0) //The condition is that distance is positive
                {
                    cluster_index.push_back(j); //Store reachable nodes
                    pathlenght += dist[j]; //Add the average distance to them
                }
            }

            //Add the contribution of this marked nodes to the average pathlenght of this cluster
            for (j=0; j < cluster_index.size(); j++)
            {
                node = cluster_index[j]; //Get the selected node
                //If we have not visited it yet (to avoid counting first node found twice)
                if (!visited[node])
                {
                    breadth_first_search(node, node_list, dist); //Compute paths from i to all other nodes and store them
                    visited[node] = true; //Mark as visited now
                    remaining -= 1; //Eliminate from remaining list
                    //We know beforehand which nodes we have to sum, because this are the reachable ones.
                    for (k=0; k < cluster_index.size(); k++)
                    {
                        pathlenght += dist[cluster_index[k]];
                    }
                }
            }

            //Finish pathlenght. Avoid single node divergence
            if (cluster_index.size() > 1)
            {
                pathlenght /= cluster_index.size() * (cluster_index.size() - 1);
            }
            else
            {
                pathlenght = 0.0;
            }

            //Check if this is max
            if (pathlenght > maxpathlenght)
            {
                maxpathlenght = pathlenght;
            }

            cluster_index.clear(); //Clear this var for another run
        }

        i += 1;
    }

    return maxpathlenght; //Return path lenght of largest component.
}

///TODO marked for revision
template <class T, typename B>
vector<int> CNetwork<T,B>::degree_distribution()
{
    int i;
    vector<int> result(current_size, 0);
    //Compute the degree distribution
    for (i=0; i < current_size; i++)
    {
        result[degree(i)] += 1;
    }
    //Erase the 0 at the end of the array
    i = current_size - 1; //Start counter
    while (result[i] == 0)
    {
        result.erase(result.begin() + i);
        i -= 1;
    }
    return result;
}

///Compute the degree correlation (also return degree distribution)
template <class T, typename B>
vector<double> CNetwork<T,B>::degree_correlation(vector<int> &distribution)
{
    int i,j,k;
    int index, numneighs;
    double mean_neigh_degree; //Average degree of the neighbour of a node
    vector<double> result;

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
    result = vector<double>(maxdegree, 0.0);
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
        result[index] += mean_neigh_degree; //Put it into the distribution
    }

    //To finish average over nodes, divide by the number of nodes with degree k
    //This are in distribution. Check that we are not dividing by 0
    for (i=0; i < result.size(); i++)
    {
        if (distribution[i] != 0) result[i] /= 1.0*distribution[i];
    }

    return result;
}

/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///


/// ======================================================================================== ///
/// =================================== Network creation =================================== ///
/// ======================================================================================== ///

template <class T, typename B>
void CNetwork<T,B>::create_erdos_renyi(int nodes, double mean_k, unsigned int random_seed)
{
    int i,j,k;
    double p = mean_k / (nodes - 1.0);

    mt19937 gen(random_seed);; //Create the generator
    uniform_real_distribution<double> ran_u(0.0,1.0); //Uniform number distribution
    uniform_int_distribution<int>  index(0,nodes-1); //-1 because closed interval for ints

    add_nodes(nodes); //Create the nodes

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
void CNetwork<T,B>::create_configurational(int nodes, int mink, double gamma, unsigned int random_seed)
{
    int i,j;
    int n_links;
    int max_size; //Maximum size if we want an uncorrelated network

    vector<int> node_degree;
    vector<int> link_vector;

    mt19937 gen(random_seed); //Create the generator
    uniform_real_distribution<double> ran_u(0.0,1.0); //Uniform number distribution

    add_nodes(nodes); //Add the nodes we need
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
void CNetwork<T,B>::create_wats_strogatz(int nodes, int num_forward_edges, double p, unsigned int random_seed)
{
    int i,j;
    int to;
    mt19937 gen(random_seed);; //Create the generator
    uniform_real_distribution<double> ran_u(0.0,1.0); //Uniform number distribution
    uniform_int_distribution<int>  index(0,nodes-1); //-1 because closed interval for ints
    vector<unsigned int> aux;
    bool eliminated;

    ///TODO check for reg_connections = 0, warn the user

    //Add the nodes
    add_nodes(nodes);

    for (i=0; i < current_size; i++)
    {
        for (j=1; j <= num_forward_edges; j++) //At least one node.
        {
            add_link(i, (i+j)%current_size); //Add the link
        }
    }


    //To avoid erasing inside the loop which may cause problems with indices
    vector<int> who_to_erase = vector<int>(0);
    vector<int> who_to_erase_index = vector<int>(0);

    for (i=0; i < current_size; i++) //To -1 because the last is already connected to the first
    {
        //Select the nodes pointed by the edges, using that we know number of edges
        for (j = num_forward_edges; j < 2 * num_forward_edges; j++)
        {
            //If selected, add to the list of rewiring
            if (ran_u(gen) <= p)
            {
                who_to_erase.push_back(i); //Add the node from
                who_to_erase_index.push_back(get_neigh_at(i,j)); //Add node to
            }
        }
    }

    //Now effectively rewire the links
    for (i=0; i < who_to_erase.size(); i++)
    {
        eliminated = remove_link(who_to_erase[i], who_to_erase_index[i]); //Try to remove the link
        do
        {
            to = index(gen); //Get a new neighbour
            aux = get_neighs(to); //Check neighs of this new neigh
        }
        //Do it again if I selected exactly the same node, or if it is myself
        while (to == who_to_erase[i] or to == who_to_erase_index[i] or (find(aux.begin(), aux.end(), who_to_erase[i]) != aux.end()) );

        if (eliminated) add_link(who_to_erase[i], to); //Add the new link
    }

    return;
}


///Creates an Albert-Barabasi free scale network
template <class T, typename B>
void CNetwork<T,B>::create_albert_barabasi(int m0, int m, unsigned int random_seed)
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
    for (i = m0; i < max_net_size; i++)
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
                //add_link(current_size-1, links[2*index(gen)+1]); //Current_size-1 because we've added a node we don't have to account for; BEFORE WAS [index][OUT]
                //add_link(current_size-1, index_add);

            }
            else
            {
                //If not, connect to a random node
                index = uniform_int_distribution<int>(0, current_size-2); //We don't want current_size-1 which is the actual node, so up to -2
                index_add = index(gen);
                //add_link(current_size-1, index_add); //Add the link
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

/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///


/// ======================================================================================== ///
/// ===================================== Getting info ===================================== ///
/// ======================================================================================== ///

///Get the degree of provided index
template <class T, typename B>
int CNetwork<T,B>::degree(int node_index)
{
    //return degree[node_index];
    return neighs[node_index].size();
}


///Get the degree of provided index
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

///Returns the link in format (from, to) as a two-component vector
template <class T, typename B>
vector<int> CNetwork<T,B>::get_link(int link_index)
{
    return {adjm[link_index].x, adjm[link_index].y};
}

///Get the weight
template <class T, typename B>
B CNetwork<T,B>::get_weight(int link_index)
{
    return adjm[link_index].value;
}

///Get how many nodes we have
template <class T, typename B>
int CNetwork<T,B>::get_node_count()
{
    return current_size;
}

///Get how many links we have
template <class T, typename B>
int CNetwork<T,B>::get_link_count()
{
    return link_count;
}

///Returns a vector of neighbors of node_index
template <class T, typename B>
vector<unsigned int> CNetwork<T,B>::get_neighs(int node_index)
{
    return neighs[node_index];
}

///Get the k neighbour of the node node_index
template <class T, typename B>
int CNetwork<T,B>::get_neigh_at(int node_index, int k)
{
    return neighs[node_index][k];
}
/*
///Returns element of the adjacency matrix, depending if
///we have selected or not a weighted network
template <class T, typename B>
int CNetwork<T,B>::get_a(int i, int j)
{
    return weighted_net ? a_w[i][j] : int(a[i][j]);
}
*/

///Set the value of a node
template <class T, typename B>
void CNetwork<T,B>::set_value(int index, T val)
{
    value[index] = val;
}

///Get the value of a node
template <class T, typename B>
T CNetwork<T,B>::get_value(int index)
{
    return value[index];
}


///This function creates an standard graphml file format, which is able to store all the data of CNetwork:
///nodes, node labels, links and weights.

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

///Following functions are overloads to set the value of a property
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

///Following functions are used to retrieve value from the properties

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


///Used to write the network as a GRAPHML file. Optional argument labels is useful to name the nodes.

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

///Write the mtx format
template <class T, typename B>
void CNetwork<T,B>::write_mtx(string filename)
{
    ofstream output;
    int i;

    output.open(filename + ".mtx");
    //Write that this is a NxN matrix with link_count links
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

/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///




/// ======================================================================================== ///
/// ============================= AUXILIARY FUNCTIONS ====================================== ///
/// ======================================================================================== ///


template <class T, typename B>
vector<double> CNetwork<T,B>::compute_eigenv(double approx_error, int max_it)
{
    return adjm.dom_eigen(approx_error, max_it);
}


/// ======================================================================================== ///
/// ======================================================================================== ///
/// ======================================================================================== ///
