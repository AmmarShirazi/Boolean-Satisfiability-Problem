#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <Windows.h>
#include <iomanip>


int get_random(int min, int max)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

std::vector<std::vector<int>> read_file(std::string file_name = "dataset.txt") {

	std::vector<std::vector<int>> data;
	std::fstream file(file_name, std::ios::in | std::ios::out);
	if (file.fail()) {
		std::cerr << "Failed to open the file." << std::endl;
		return {};
	}

	int i = 0;
	std::vector<int> temp_vector;
	data.push_back(temp_vector);

	while (!file.eof()) {
		
		std::string temp_buffer;
		file >> temp_buffer;


		if (temp_buffer == "0") {
			i++;
			std::vector<int> temp_vector__;
			data.push_back(temp_vector__);
			continue;
		}
		
		data[i].push_back(std::stoi(temp_buffer));
		
	}
	static_cast<void>(data.pop_back());

	return data;
}

bool* generate_random_assignment(int size) {

	bool* assignment = new bool[size];
	for (size_t i = 0; i < size; i++) {
		assignment[i] = static_cast<bool>(get_random(0, 1));
	}

	return assignment;
}

void evaluate_clauses(std::vector<std::vector<int>> data, bool* truth_set, int clauses, bool* assignment, int variables) {


	for (int i = 0; i < data.size(); i++) {
		std::vector<int> temp_values;
		for (int j = 0; j < data[i].size(); j++) {
			
			int temp_index = data[i][j];
			int temp_truth_val = assignment[abs(temp_index)];
			if (temp_index < 0) {
				temp_truth_val = !temp_truth_val;
			}
			temp_values.push_back(temp_truth_val);
		}
		truth_set[i] = false;
		for (int k = 0; k < temp_values.size(); k++) {
			if (temp_values[k] == 1) {
				truth_set[i] = true;
				break;
			}
		}
		// std::cout << i << ": " << truth_set[i] << std::endl;
	}

}

int get_truth_count(bool* truth_set, int clauses) {

	int count = 0;
	for (int i = 0; i < clauses; i++) {
		if (truth_set[i] == true) {
			count++;
		}
	}

	return count;
}

int get_random_false_clause(bool* truth_set, int clauses) {

	std::vector<int> random_clause;
	for (int i = 0; i < clauses; i++) {
		if (truth_set[i] == false) {
			random_clause.push_back(i);
		}
	}

	return random_clause[get_random(0, static_cast<int>(random_clause.size() - 1))];
}

int compute_break_count(std::vector<std::vector<int>> data, bool* original_truth_set, bool* assignment , int clauses, int break_index) { // break -> true clauses that become false


	int break_count = 0;
	bool* truth_set = new bool[clauses];
	for (int i = 0; i < data.size(); i++) {
		std::vector<int> temp_values;
		for (int j = 0; j < data[i].size(); j++) {

			int temp_index = data[i][j];
			int temp_truth_val = assignment[abs(temp_index)];
			if (abs(temp_index) == abs(break_index)) {
				temp_truth_val = !temp_truth_val;
			}
			if (temp_index < 0) {
				temp_truth_val = !temp_truth_val;
			}
			temp_values.push_back(temp_truth_val);
		}
		truth_set[i] = false;
		for (int k = 0; k < temp_values.size(); k++) {
			if (temp_values[k] == 1) {
				truth_set[i] = true;
				break;
			}
		}
		if (original_truth_set[i] == true && truth_set[i] == false) {
			break_count++;
		}
		// std::cout << i << ": " << truth_set[i] << std::endl;
	}

	delete[] truth_set;
	return break_count;
}

bool* copy_assignment(bool* assignment, int variables) {

	bool* new_assignment = new bool[variables];

	for (int i = 0; i < variables; i++) {
		new_assignment[i] = assignment[i];
	}

	return new_assignment;
}

int count_difference_in_assignment(bool* assignment1, bool* assignment2, int variables) {

	int count = 0;

	for (int i = 0; i < variables; i++) {
		if (assignment1[i] != assignment2[i]) {
			count++;
		}
	}

	return count;
}


void print_clause(std::vector<int> clause) {

	std::cout << "[";
	for (int i = 0; i < clause.size(); i++) {
		if (i == clause.size() - 1) {
			std::cout << clause[i];
		}
		else {
			std::cout << clause[i] << ", ";
		}
	}
	std::cout << "]" << std::endl;

}


bool* generate_new_assignment(std::vector<std::vector<int>> data, bool* truth_set, int clauses, bool* assignment, int variables, int k_max, int original_truth_count) {

	for (int i = 0; i < 1000; i++) {
		
		bool* new_assignment = copy_assignment(assignment, variables);
		bool* new_truth_set = copy_assignment(truth_set, clauses);

		for (;;) { // break condition at bottom, loop runs to the difference

			int false_clause = get_random_false_clause(new_truth_set, clauses);
			std::cout << "\nCurrent Working On Clause #: " << false_clause << " ";
			print_clause(data[false_clause]);

			std::vector<int> break_counts;
			for (int j = 0; j < data[false_clause].size(); j++) {
				break_counts.push_back(compute_break_count(data, new_truth_set, new_assignment, clauses, data[false_clause][j]));
			}
			bool zero_break_count_flag = false;
			int break_count_zero_flip = -1;
			for (int j = 0; j < break_counts.size(); j++) {
				if (break_counts[j] == 0) {
					zero_break_count_flag = true;
					break_count_zero_flip = abs(data[false_clause][j]);
				}
			}
			if (break_count_zero_flip != -1) {
				std::cout << "Flipping Variable #: " << break_count_zero_flip  << " From " << new_assignment[break_count_zero_flip] << " To " << !new_assignment[break_count_zero_flip] << std::endl;
				new_assignment[break_count_zero_flip] = !new_assignment[break_count_zero_flip];
				
			}
			else {
				int var = 0;
				int p = get_random(0, 100);
				if (p > 65) {
					var = get_random(0, static_cast<int>(break_counts.size() - 1));
				}
				else {
					var = 0;
					for (int j = 0; j < break_counts.size(); j++) {
						if (break_counts[var] < break_counts[j]) {
							var = j;
						}
					}
				}

				int to_flip = abs(data[false_clause][var]);
				std::cout << "Flipping Variable #: " << to_flip << " From " << new_assignment[to_flip] << " To " << !new_assignment[to_flip] << std::endl;
				new_assignment[to_flip] = !new_assignment[to_flip];
			}

			evaluate_clauses(data, new_truth_set, clauses, new_assignment, variables);
			int new_truth_count = get_truth_count(new_truth_set, clauses);
			
			if (count_difference_in_assignment(assignment, new_assignment, variables) > k_max) {
				delete[] new_assignment;
				delete[] new_truth_set;
				break;
			}
			if (new_truth_count > original_truth_count) {
				delete[] new_truth_set;
				return new_assignment;
			}
		}
	}
	
	return nullptr;
}

void write_assignment_to_file(std::string filename, bool* assignment, int variables) {


	std::fstream file(filename, std::ios::in | std::ios::out | std::ios::trunc);
	if (file.fail()) {
		std::cerr << "Failed to open the file." << std::endl;
		return;
	}
	for (int i = 1; i < variables; i++) {
		file << static_cast<int>(assignment[i]) << std::endl;
	}
}

int main() {

	std::vector<std::vector<int>> data = read_file("dataset.txt");
	const int clauses = static_cast<int>(data.size());

	int k = 0;
	int variables = 0;
	
	

	std::cout << "Input number of variables: ";
	std::cin >> variables;
	variables++;

	std::cout << "Input K: ";
	std::cin >> k;


	bool* truth_set = new bool[clauses];
	bool* assignment = generate_random_assignment(variables);

	write_assignment_to_file("TRandom.txt", assignment, variables);
	evaluate_clauses(data, truth_set, clauses, assignment, variables);
	int original_truth_count = get_truth_count(truth_set, clauses);
	std::cout << "The Original Truth Count For Assignment T: ";
	std::cout << original_truth_count << std::endl; // processing of initial T

	clock_t start, end;
	start = clock();
	bool* new_assignment = generate_new_assignment(data, truth_set, clauses, assignment, variables, k, original_truth_count);
	end = clock();

	bool* new_truth_set = new bool[clauses];
	evaluate_clauses(data, new_truth_set, clauses, new_assignment, variables);
	int new_truth_count = get_truth_count(new_truth_set, clauses);
	std::cout << "\nThe New Truth Count For Assignment T: ";
	std::cout << new_truth_count << std::endl; // processing of new T

	std::cout << "Difference From Original T: " << count_difference_in_assignment(assignment, new_assignment, variables) << std::endl;
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
	std::cout << "Time Taken To Compute Result: " << std::fixed << std::setprecision(5) << time_taken << std::endl;
	write_assignment_to_file("TDash.txt", new_assignment, variables);

	return 0;
}
