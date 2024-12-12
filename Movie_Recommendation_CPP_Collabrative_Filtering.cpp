#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;

// Load the ratings matrix from a CSV file
vector<vector<int>> loadRatingsMatrix(const string& filename) {
    vector<vector<int>> ratingsMatrix;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(stoi(cell));
        }
        ratingsMatrix.push_back(row);
    }
    return ratingsMatrix;
}

// Calculate cosine similarity between two users
float calculateSimilarity(const vector<int>& user1, const vector<int>& user2) {
    float dotProduct = 0.0, norm1 = 0.0, norm2 = 0.0;

    for (size_t i = 0; i < user1.size(); ++i) {
        if (user1[i] != 0 && user2[i] != 0) { // Only consider rated movies
            dotProduct += user1[i] * user2[i];
        }
        norm1 += user1[i] * user1[i];
        norm2 += user2[i] * user2[i];
    }

    if (norm1 == 0 || norm2 == 0) return 0.0;
    return dotProduct / (sqrt(norm1) * sqrt(norm2));
}

// Predict ratings for a given user
vector<float> predictRatings(const vector<vector<int>>& ratingsMatrix, int userIndex) {
    vector<float> predictedRatings(ratingsMatrix[0].size(), 0.0);
    vector<float> similaritySum(ratingsMatrix[0].size(), 0.0);

    for (size_t otherUser = 0; otherUser < ratingsMatrix.size(); ++otherUser) {
        if (otherUser == userIndex) continue;

        float similarity = calculateSimilarity(ratingsMatrix[userIndex], ratingsMatrix[otherUser]);

        for (size_t movie = 0; movie < ratingsMatrix[0].size(); ++movie) {
            if (ratingsMatrix[userIndex][movie] == 0 && ratingsMatrix[otherUser][movie] != 0) {
                predictedRatings[movie] += similarity * ratingsMatrix[otherUser][movie];
                similaritySum[movie] += fabs(similarity);
            }
        }
    }

    for (size_t movie = 0; movie < predictedRatings.size(); ++movie) {
        if (similaritySum[movie] != 0) {
            predictedRatings[movie] /= similaritySum[movie];
        }
    }

    return predictedRatings;
}

// Recommend top N movies
vector<pair<int, float>> recommendMovies1(const vector<float>& predictedRatings, int N) {
    vector<pair<int, float>> movieScores;

    for (size_t i = 0; i < predictedRatings.size(); ++i) {
        if (predictedRatings[i] > 0) {
            movieScores.push_back({i, predictedRatings[i]});
        }
    }

    sort(movieScores.begin(), movieScores.end(), [](const pair<int, float>& a, const pair<int, float>& b) {
        return a.second > b.second; // Sort by descending scores
    });

    if (movieScores.size() > N) {
        movieScores.resize(N);
    }

    return movieScores;
}

// Recommend top N movies (rated + unrated)
vector<pair<int, float>> recommendMovies2(const vector<float>& predictedRatings, const vector<int>& userRatings, int topN) {
    vector<pair<int, float>> movieScores;

    // Combine both rated and predicted ratings
    for (int i = 0; i < predictedRatings.size(); ++i) {
        if (userRatings[i] != 0) {
            // Use the actual user rating for already-rated movies
            movieScores.push_back({i, static_cast<float>(userRatings[i])});
        } else {
            // Use the predicted rating for unrated movies
            movieScores.push_back({i, predictedRatings[i]});
        }
    }

    // Sort the movie scores in descending order by rating
    sort(movieScores.begin(), movieScores.end(), [](const pair<int, float>& a, const pair<int, float>& b) {
        return a.second > b.second;
    });

    // Keep only the top N recommendations
    if (movieScores.size() > topN) {
        movieScores.resize(topN);
    }

    return movieScores;
}

// Count the number of unrated movies for a given user
int countUnratedMovies(const vector<int>& userRatings) {
    int count = 0;
    for (int rating : userRatings) {
        if (rating == 0) {
            count++;
        }
    }
    return count;
}

// Calculate RMSE
float calculateRMSE(const vector<vector<int>>& actualRatings, const vector<float>& predictedRatings, int userIndex) {
    float sumSquaredError = 0.0;
    int count = 0;

    for (size_t movie = 0; movie < actualRatings[userIndex].size(); ++movie) {
        if (actualRatings[userIndex][movie] != 0) { // Only consider rated movies
            float error = actualRatings[userIndex][movie] - predictedRatings[movie];
            sumSquaredError += error * error;
            count++;
        }
    }

    return (count > 0) ? sqrt(sumSquaredError / count) : 0.0;
}

// Handle edge cases for users with no ratings or movies with no ratings
bool validateRatingsMatrix(const vector<vector<int>>& ratingsMatrix) {
    if (ratingsMatrix.empty() || ratingsMatrix[0].empty()) {
        cerr << "Error: Ratings matrix is empty or malformed." << endl;
        return false;
    }

    for (const auto& userRatings : ratingsMatrix) {
        if (all_of(userRatings.begin(), userRatings.end(), [](int rating) { return rating == 0; })) {
            cerr << "Error: At least one user has no rated movies." << endl;
            return false;
        }
    }

    for (size_t movie = 0; movie < ratingsMatrix[0].size(); ++movie) {
        bool hasRating = false;
        for (size_t user = 0; user < ratingsMatrix.size(); ++user) {
            if (ratingsMatrix[user][movie] != 0) {
                hasRating = true;
                break;
            }
        }
        if (!hasRating) {
            cerr << "Error: At least one movie has no ratings." << endl;
            return false;
        }
    }

    return true;
}

int main() {
    string filename = "ratings.csv";
    vector<vector<int>> ratingsMatrix = loadRatingsMatrix(filename);

    if (!validateRatingsMatrix(ratingsMatrix)) {
        return 1;
    }

    int userIndex;
    int topN;

    cout << "Enter user index (1-based): ";
    cin >> userIndex;

    // Check if userIndex is within bounds
    if (userIndex < 1 || userIndex > ratingsMatrix.size()) {
        cerr << "Error: User index out of bounds. Please enter a value between 1 and " << ratingsMatrix.size() << "." << endl;
        return 1;
    }
    userIndex -= 1; // Convert to 0-based index for internal processing

    cout << "Enter number of top recommendations: ";
    cin >> topN;

    // Check if topN is valid
    if (topN < 1 || topN > ratingsMatrix[userIndex].size()) {
        cerr << "Error: Invalid value for number of top recommendations. Please enter a value between 1 and " << ratingsMatrix[userIndex].size() << "." << endl;
        return 1;
    }

    // Predict ratings for the given user
    vector<float> predictedRatings = predictRatings(ratingsMatrix, userIndex);

    // Calculate RMSE for the user
    float rmse = calculateRMSE(ratingsMatrix, predictedRatings, userIndex);

    // Display predicted ratings for all unrated movies
    cout << fixed << setprecision(2);
    cout << "\nPredicted ratings for unrated movies for User " << userIndex + 1 << ":\n";
    for (size_t movie = 0; movie < ratingsMatrix[userIndex].size(); ++movie) {
        if (ratingsMatrix[userIndex][movie] == 0) {  // Only show unrated movies
            cout << "Movie " << movie + 1 << ": " << predictedRatings[movie] << endl;
        }
    }

    // Count and display the number of unrated movies
    int unratedCount = countUnratedMovies(ratingsMatrix[userIndex]);

    // Recommend the top N movies (unrated)
    vector<pair<int, float>> recommendations1 = recommendMovies1(predictedRatings, topN);

    // Display the top N recommendations (unrated only) 
    // cout << "\nTop " << unratedCount << " recommended movies for User " << userIndex + 1 << " (Unrated Movies Only):\n";
    cout << "\nTop" << " recommended movies for User " << userIndex + 1 << " (Unrated Movies Only):\n";
    for (const auto& rec : recommendations1) {
        cout << "Movie " << rec.first + 1 << " with predicted rating: " << rec.second << endl;
    }

    vector<pair<int, float>> recommendations2 = recommendMovies2(predictedRatings, ratingsMatrix[userIndex], topN);

    // Display the top N recommendations (rated + unrated)
    cout << "\nTop " << topN << " recommended movies for User " << userIndex + 1 << " (Including Both Rated and Unrated Movies):\n";
    for (const auto& rec : recommendations2) {
        cout << "Movie " << rec.first + 1 << " with score: " << rec.second << endl;
    }

    // Display RMSE
    cout << "\nRMSE for User " << userIndex + 1 << ": " << rmse << endl;

    return 0;
}

