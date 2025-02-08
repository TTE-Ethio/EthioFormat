#include <iostream>
#include <vector>
#include <string>

using namespace std;

class EthioTreeNode {
	public:
		std::string GetKey(int index);
		std::string GetValue(int index);
		void AddKeyValuePair(std::string key, std::string value);
		EthioTreeNode* GetChild(int nth);
		EthioTreeNode* GetParent();
		int GetNumberOfChildren(){ return this->children.size(); }
		int GetNumKVPs(){ return numKVPs; }
		int GetDepth(){ return depth; }
		int GetWhichChild(){ return nthChild; }
	private:
	    std::vector<std::string> keys;      // Holds the keys for this node
	    std::vector<std::string> values;    // Holds the corresponding values
	    std::vector<EthioTreeNode*> children; // Holds pointers to child nodes
	    EthioTreeNode* parent;             // Pointer to the parent node
	    int numKVPs;
	    int depth;
	    int nthChild;
	public:
	    // Constructor
	    EthioTreeNode(EthioTreeNode* parentNode = NULL, int depth=0) {
	    	parent = parentNode;
	    	numKVPs = 0;
	    	if(parentNode){
	    		parentNode->children.push_back(this);
	    		this->depth = depth;
	    		nthChild = parentNode->GetNumberOfChildren();
	    	}
	    }
};

// Returns a key at the given index
std::string EthioTreeNode::GetKey(int index) {
    if (index >= 0 && index < keys.size()) {
        return keys[index];
    }
    return ""; // Return empty string if index is out of range
}
// Returns a value at the given index
std::string EthioTreeNode::GetValue(int index) {
    if (index >= 0 && index < values.size()) {
        return values[index];
    }
    return ""; // Return empty string if index is out of range
}

// Adds a key-value pair to the node
void EthioTreeNode::AddKeyValuePair(std::string key, std::string value) {
    keys.push_back(key);
    values.push_back(value);
    this->numKVPs++;
}
// Returns a pointer to the nth child node if within range
EthioTreeNode* EthioTreeNode::GetChild(int nth) {
    if (nth >= 0 && nth < children.size()) {
        return children[nth];
    }
    return NULL; // Return NULL if index is out of range
}

// Returns a pointer to the parent node
EthioTreeNode* EthioTreeNode::GetParent() {
    return parent;
}

class EthioFormatB{
private:
	void UtilSerializeEthioFormatMap(EthioTreeNode* rootNode, vector<int>& map){
		map.push_back(1);
		for(int i = 0; i < rootNode->GetNumKVPs(); i++){ map .push_back(2); }
		for(int i = 0; i < rootNode->GetNumberOfChildren(); i++){
			UtilSerializeEthioFormatMap(rootNode->GetChild(i), map);
		}
		map.push_back(0);
	}
	int GetBinaryOfCharacter(char character){

	}
	
	void UtilSerializeEthioFormatKVPs(EthioTreeNode* rootNode, vector<int>& kvpBinaryNumbers){
		for(int i = 0; i < rootNode->GetNumKVPs(); i++){
			kvpString += (rootNode->GetKey(i) +  + rootNode->GetValue(i) + "\n");
		}
		for(int i = 0; i < rootNode->GetNumberOfChildren(); i++){
			UtilSerializeEthioFormatKVPs(rootNode->GetChild(i), kvpString);
		}
	}

	void TraverseEthioTreeForPointerCollection(EthioTreeNode* etn){
		tempEtnPointers.push_back(etn);
		for(int i = 0; i < etn->GetNumberOfChildren(); i++){
			TraverseEthioTreeForPointerCollection(etn->GetChild(i));
		}
	}

	vector<string> tempKeys;
	vector<string> tempValues;
	vector<EthioTreeNode*> tempEtnPointers;
public:
	EthioTreeNode* ParseEthioFormatT(std::string rawEthioFormat) {
    	// Step 1: Parse the raw string into a table
	    std::vector<std::vector<std::string>> table = ParseRawString(rawEthioFormat, 2);
            // A size of 0 of the table indicates an error or empty
	    if(table.size() == 0){ return NULL);
	    std::vector<std::string> firstKVP = table[0];  // First vector is the building map
	    string buildingMap = firstKVP[0];
	    int tableIndex = 1;  // Start from 1 since index 0 is the building map
	    EthioTreeNode* rootNode = new EthioTreeNode(NULL);  // Root node
	    EthioTreeNode* currentNode = rootNode;  // Start with root node
	    int depth = 0;
	    // Step 2: Iterate over the building map (the first vector)
	    for (size_t i = 0; i < buildingMap.size(); ++i) {
	        char mapValue = buildingMap[i];  // The character indicating the operation
	        // Conditional logic based on the mapValue
	        if (mapValue == '1') {  // Move deeper and create a new node
	            depth++;
	            EthioTreeNode* newNode = new EthioTreeNode(currentNode, depth);  // Create a new node with currentNode as parent
	            currentNode = newNode;  // Set currentNode to newNode
	        } else if (mapValue == '0') {  // Backtrack to the parent
	            currentNode = currentNode->GetParent();
	            depth--;
	        } else if (mapValue == '2') {  // Key-Value pair
	        	EthioTreeNode* newNode = NULL;  // Temporary node pointer
	            currentNode->AddKeyValuePair(table[tableIndex][0], table[tableIndex][1]);  // Add key-value
	            ++tableIndex;  // Move to the next entry in the table
	        }
	    }
	    // Step 3: Return the root node after processing all the entries
	    return rootNode;
	}
	string SerializeEthioFormat(EthioTreeNode* rootNode){
		string treeBuildingMap = "";
		UtilSerializeEthioFormatMap(rootNode, treeBuildingMap);
		string kvpString = "";
		UtilSerializeEthioFormatKVPs(rootNode, kvpString);
		return treeBuildingMap + "\n\n" + kvpString + "\0";
	}
	void TraverseEthioTree(EthioTreeNode* rootNode, void(*TakeActionOnNode)(vector<string>& keys, vector<string>& values, int depth, int whichChild)){
		for(int i = 0; i < rootNode->GetNumKVPs(); i++){
			tempKeys.push_back(rootNode->GetKey(i));
			tempValues.push_back(rootNode->GetValue(i));
		}
		TakeActionOnNode(tempKeys, tempValues, rootNode->GetDepth(), rootNode->GetWhichChild());
		tempKeys.clear();
		tempValues.clear();
		for(int i = 0; i < rootNode->GetNumberOfChildren(); i++){
			TraverseEthioTree(rootNode->GetChild(i), TakeActionOnNode);
		}
	}
	void CleanUpEthioTree(EthioTreeNode* rootNode){
		TraverseEthioTreeForPointerCollection(rootNode);
		for(int i = 0; i < tempEtnPointers.size(); i++){
			delete tempEtnPointers[i];
		}
		tempEtnPointers.clear();
	}
};

void PrintKVPs(vector<string>& keys, vector<string>& values, int depth, int whichChild){
	cout << "Depth: " << depth << " Which Child: " << whichChild << endl;
	for(int i = 0; i < keys.size(); i++){
		cout << keys[i] << " : " << values[i] << endl;
	}
	cout << endl;
}
int main(){
	EthioFormat ef;
	/*
			Node-1
   		        /|\
	     	       / | \
	      	      /  |  \
	             /   |   \
	            /    |    \
	           /     |     \
	      Node-2  Node-3   Node-7 (k10->v10
	     (k1->v1  (k3->v3)  |\     k11->k11)
             k2->v2)   /|\      | \
		      / | \     |   \
		    /   |  \   Node-8 \ 
      		   /    |    \(k12->v12)\
	         /	|     \           \
	       Node-4  Node-5   Node-6     Node-9
 	      (k4->v4  (k7->v7) (k8->v8	   (k13->v13
              k5->k5		k9->k9)     k14->v14)
              k6->k6)
  	
   	*/
	//Since this is the binary version, we will write the numerical representation alongside the offset
	EthioTreeNode* etn = ef.ParseEthioFormatT( "1220121222012012200122120122\n\nk1\nv1\nk2\nv2\nk3\nv3\nk4\nv4\nk5\nv5\nk6\nv6\nk7\nv7\nk8\nv8\nk9\nv9\nk10\nv10\nk11\nv11\nk12\nv12\nk13\nv13\nk14\nv14\n\0");
	if(!etn){
		cout << "Invalid or empty content" << endl;
	} else{
		ef.TraverseEthioTree(etn, PrintKVPs);
		string serializedEthioFormat = ef.SerializeEthioFormat(etn);
		cout << serializedEthioFormat;
		ef.CleanUpEthioTree(etn);
	}
	return 0;
}
