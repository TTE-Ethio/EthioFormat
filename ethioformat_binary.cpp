#include <iostream>
#include <vector>
#include <string>

using namespace std;

class EthioTreeNodeB {
public:
	std::vector<int> GetKey(int index);
	std::vector<int> GetValue(int index);
	void AddKeyValuePair(std::vector<int>, std::vector<int> value);
	EthioTreeNodeB* GetChild(int nth);
	EthioTreeNodeB* GetParent();
	int GetNumberOfChildren() { return this->children.size(); }
	int GetNumKVPs() { return numKVPs; }
	int GetDepth() { return depth; }
	int GetWhichChild() { return nthChild; }
private:
	std::vector<vector<int>> keys;      // Holds the keys for this node
	std::vector<vector<int>> values;    // Holds the corresponding values
	std::vector<EthioTreeNodeB*> children; // Holds pointers to child nodes
	EthioTreeNodeB* parent = NULL;             // Pointer to the parent node
	int numKVPs;
	int depth = 0;
	int nthChild;
public:
	// Constructor
	EthioTreeNodeB(EthioTreeNodeB* parentNode = NULL, int depth = 0) {
		parent = parentNode;
		numKVPs = 0;
		if (parentNode) {
			parentNode->children.push_back(this);
			this->depth = depth;
			nthChild = parentNode->GetNumberOfChildren();
		}
	}
};

// Returns a key at the given index
std::vector<int> EthioTreeNodeB::GetKey(int index) {
	if (index >= 0 && index < keys.size()) {
		return keys[index];
	}
}
// Returns a value at the given index
std::vector<int> EthioTreeNodeB::GetValue(int index) {
	if (index >= 0 && index < values.size()) {
		return values[index];
	}
}

// Adds a key-value pair to the node
void EthioTreeNodeB::AddKeyValuePair(std::vector<int> key, std::vector<int> value) {
	keys.push_back(key);
	values.push_back(value);
	this->numKVPs++;
}
// Returns a pointer to the nth child node if within range
EthioTreeNodeB* EthioTreeNodeB::GetChild(int nth) {
	if (nth >= 0 && nth < children.size()) {
		return children[nth];
	}
	return NULL; // Return NULL if index is out of range
}

// Returns a pointer to the parent node
EthioTreeNodeB* EthioTreeNodeB::GetParent() {
	return parent;
}

class EthioFormatB {
private:
	void GetBinaryOfString(string singleFieldText, vector<int>& singleFieldBinary) {
		singleFieldBinary.clear();
		for (int i = 0; i < singleFieldText.size(); i++) {
			singleFieldBinary.push_back(int(singleFieldText[i]));
		}
	}

	void UtilSerializeEthioFormatMap(EthioTreeNodeB* rootNode, vector<int>& map) {
		map.push_back(1);
		for (int i = 0; i < rootNode->GetNumKVPs(); i++) { map.push_back(2); }
		for (int i = 0; i < rootNode->GetNumberOfChildren(); i++) {
			UtilSerializeEthioFormatMap(rootNode->GetChild(i), map);
		}
		map.push_back(0);
	}
	void UtilSerializeEthioFormatKVPsSizes(EthioTreeNodeB* rootNode, vector<int>& sizeOfAllKVPs) {
		for (int i = 0; i < rootNode->GetNumKVPs(); i++) {
			sizeOfAllKVPs.push_back(rootNode->GetKey(i).size());
			sizeOfAllKVPs.push_back(rootNode->GetValue(i).size());
		}
		for (int i = 0; i < rootNode->GetNumberOfChildren(); i++) {
			UtilSerializeEthioFormatKVPsSizes(rootNode->GetChild(i), sizeOfAllKVPs);
		}
	}
	void UtilSerializeEthioFormatKVPs(EthioTreeNodeB* rootNode, vector<int>& kvpBinaryNumbers) {
		for (int i = 0; i < rootNode->GetNumKVPs(); i++) {
			vector<int> currentKey = rootNode->GetKey(i);
			vector<int> currentValue = rootNode->GetValue(i);

			for (int j = 0; j < currentKey.size(); j++) {
				kvpBinaryNumbers.push_back(int(currentKey[j]));
			}
			for (int j = 0; j < currentValue.size(); j++) {
				kvpBinaryNumbers.push_back(int(currentValue[j]));
			}
		}
		for (int i = 0; i < rootNode->GetNumberOfChildren(); i++) {
			UtilSerializeEthioFormatKVPs(rootNode->GetChild(i), kvpBinaryNumbers);
		}
	}

	void TraverseEthioTreeForPointerCollection(EthioTreeNodeB* etn) {
		tempEtnPointers.push_back(etn);
		for (int i = 0; i < etn->GetNumberOfChildren(); i++) {
			TraverseEthioTreeForPointerCollection(etn->GetChild(i));
		}
	}

	vector<vector<int>> tempKeys;
	vector<vector<int>> tempValues;
	vector<EthioTreeNodeB*> tempEtnPointers;
public:
	EthioTreeNodeB* ParseEthioFormatB(vector<int>& rawEthioFormat) {
		vector<int> mapBinaryNumbers;
		vector<int> sizesOfAllKVPs;
		vector<vector<int>> allKeyBinaryValues;
		vector<vector<int>> allValueBinaryValues;

		int sizeOfKVPSizes = 0;
		int refIndex = 0;

		for (; rawEthioFormat[refIndex] != 51; refIndex++) {
			mapBinaryNumbers.push_back(rawEthioFormat[refIndex]);
			if (mapBinaryNumbers[refIndex] == 50) {
				sizeOfKVPSizes++;
			}
		} refIndex++;

		for (int i = 0; i < sizeOfKVPSizes; i++) {
			sizesOfAllKVPs.push_back(rawEthioFormat[refIndex++]);
			sizesOfAllKVPs.push_back(rawEthioFormat[refIndex++]);
		}

		for (int sizeIndex = 0; sizeIndex < sizesOfAllKVPs.size(); ) {
			int keySize = sizesOfAllKVPs[sizeIndex++];
			int valueSize = sizesOfAllKVPs[sizeIndex++];

			vector<int> singleKey;
			vector<int> singleValue;

			for (int i = 0; i < keySize; i++) {
				singleKey.push_back(rawEthioFormat[refIndex++]);
			}
			for (int i = 0; i < valueSize; i++) {
				singleValue.push_back(rawEthioFormat[refIndex++]);
			}

			allKeyBinaryValues.push_back(singleKey);
			allValueBinaryValues.push_back(singleValue);
		}


		// A size of 0 of the table indicates an error or empty
		if (allKeyBinaryValues.size() == 0) {
			return NULL;
		}

	//int tableIndex = 1;  // Start from 1 since index 0 is the building map
	EthioTreeNodeB* rootNode = new EthioTreeNodeB(NULL);  // Root node
	EthioTreeNodeB* currentNode = rootNode;  // Start with root node
	int depth = 0;


	for (int i = 0, tableIndex = 0; i < mapBinaryNumbers.size(); ++i) {
		int mapValue = mapBinaryNumbers[i]; // The number indicating the operation
		// Conditional logic based on the mapValue
		if (mapValue == 49) {  // Move deeper and create a new node
			depth++;
			EthioTreeNodeB* newNode = new EthioTreeNodeB(currentNode, depth);  // Create a new node with currentNode as parent
			currentNode = newNode;  // Set currentNode to newNode
		}
		else if (mapValue == 48) {  // Backtrack to the parent
			currentNode = currentNode->GetParent();
			depth--;
		}
		else if (mapValue == 50) {  // Key-Value pair
			EthioTreeNodeB* newNode = NULL;  // Temporary node pointer
			currentNode->AddKeyValuePair(allKeyBinaryValues[tableIndex], allValueBinaryValues[tableIndex]);  // Add key-value
			++tableIndex;  // Move to the next entry in the table
		}
	}
	// Step 3: Return the root node after processing all the 
	return rootNode;
}

vector<int> SerializeEthioFormat(EthioTreeNodeB* rootNode) {
	vector<int> treeBuildingContent;
	UtilSerializeEthioFormatMap(rootNode, treeBuildingContent);
	UtilSerializeEthioFormatKVPsSizes(rootNode, treeBuildingContent);
	UtilSerializeEthioFormatKVPs(rootNode, treeBuildingContent);
	return treeBuildingContent;
}
void TraverseEthioTree(EthioTreeNodeB* rootNode, void(*TakeActionOnNode)(vector<vector<int>>& keys, vector<vector<int>>& values, int depth, int whichChild)) {
	for (int i = 0; i < rootNode->GetNumKVPs(); i++) {
		tempKeys.push_back(rootNode->GetKey(i));
		tempValues.push_back(rootNode->GetValue(i));
	}
	TakeActionOnNode(tempKeys, tempValues, rootNode->GetDepth(), rootNode->GetWhichChild());
	tempKeys.clear();
	tempValues.clear();
	for (int i = 0; i < rootNode->GetNumberOfChildren(); i++) {
		TraverseEthioTree(rootNode->GetChild(i), TakeActionOnNode);
	}
}
void CleanUpEthioTree(EthioTreeNodeB* rootNode) {
	TraverseEthioTreeForPointerCollection(rootNode);
	for (int i = 0; i < tempEtnPointers.size(); i++) {
		delete tempEtnPointers[i];
	}
	tempEtnPointers.clear();
}
};

void PrintKVPs(vector<vector<int>>& keys, vector<vector<int>>& values, int depth, int whichChild) {
	cout << "Depth: " << depth << " Which child " << whichChild << " ";
	for (int i = 0; i < keys.size(); i++) {
		vector<int> key = keys[i];
		vector<int> value = values[i];

		cout << "Key: ";
		for (int j = 0; j < key.size(); j++) {
			printf("%c", key[j]);
		} printf(" ");
		cout << "Value: ";
			for (int j = 0; j < value.size(); j++) {
				printf("%c", value[j]);
			} printf(" ");
		}
		cout << endl;
	}
	int main() {
		EthioFormatB efb;
		/*
						Node-1
						/|\
					   / | \
					  /  |  \
					 /   |   \
					/    |    \
				   /     |     \
			  Node-2  Node-3   Node-7 (k10->v10
			 (k1->v1  (kZ3->v3)  |\     k11->k11)
			 k2->v2)   /|\      | \
			     	  / | \     |   \
					/   |  \  Node-8 \
				   /    |   \(k12->v12)\
				  /	    |     \         \
			Node-4  Node-5 Node-6     Node-9
			 (k4->v4  (k7->v7) (k8->v8)	 (k13->v13
			  k5->k5           k9->k9)	  k14->v14)
			  k6->k6)

		*/


		// 1220121222012012200122120122\n\nk1\nv1\nk2\nv2\nk3\nv3\nk4\nv4\nk5\nv5\nk6\nv6\nk7\nv7\nk8\nv8\nk9\nv9\nk10\nv10\nk11\nv11\nk12\nv12\nk13\nv13\nk14\nv14\n\0
		string map = "12201212220120122001221201223";
		vector<int> binaryVersion;
		for (int i = 0; i < map.size(); i++) {
			binaryVersion.push_back(int(map[i]));
		}
		vector<int> sizes = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
		for (int i = 0; i < sizes.size(); i++) {
			binaryVersion.push_back(sizes[i]);
		}
		vector<string> actualContents = { "k1", "v1", "k2", "v2", "k3", "v3", "k4", "v4", "k5", "v5", "k6", "v6", "k7", "v7", "k8", "v8", "k9", "v9", "k10", "v10", "k11", "v11", "k12", "v12", "k13", "v13", "k14", "v14" };
		for (int i = 0; i < actualContents.size(); i++) {
			for (int j = 0; j < actualContents[i].size(); j++) {
				binaryVersion.push_back(int(actualContents[i][j]));
			}
		}
		EthioTreeNodeB* etn = efb.ParseEthioFormatB(binaryVersion);

		for (int i = 0; i < actualContents.size(); i++) {
			for (int j = 0; j < actualContents[i].size(); j++) {
				binaryVersion.push_back(int(actualContents[i][j]));
			}
		}

		if (!etn) {
			cout << "Invalid or empty content" << endl;
		}
		else {
			efb.TraverseEthioTree(etn, PrintKVPs);
			// Serialize
			efb.CleanUpEthioTree(etn);
		}
		return 0;
	}
