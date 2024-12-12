# Movie Recommendation System

This project implements a movie recommendation system using collaborative filtering. It predicts ratings for unrated movies based on user similarity and provides recommendations for movies to watch.

## Features
- Load a ratings matrix from a CSV file.
- Predict movie ratings for a specific user using cosine similarity.
- Recommend the top `N` movies for a user, based on:
  - Unrated movies only.
  - Both rated and unrated movies.
- Compute RMSE (Root Mean Squared Error) to evaluate prediction accuracy.
- Handle edge cases such as users with no ratings or movies with no ratings.

## Getting Started

### Prerequisites
- A C++ compiler supporting C++11 or higher.
- Input file `ratings.csv` containing the ratings matrix.
- The ratings matrix should follow this format:
  - Each row represents a user.
  - Each column represents a movie.
  - Ratings are integers (e.g., 1–5), with `0` indicating an unrated movie.

### Steps to Run the Program

1. Clone this repository to your local machine.
```bash
$ git clone <repository-url>
$ cd <repository-folder>
```

2. Compile the program.
```bash
$ g++ -std=c++11 -o recommender recommender.cpp
```

3. Prepare your `ratings.csv` file.
   - Place the CSV file in the same directory as the executable.

4. Run the program.
```bash
$ ./recommender
```

5. Follow the prompts:
   - Enter the user index (1-based index).
   - Enter the number of top recommendations to display.

### Example Input and Output

#### Input:
```
Enter user index (1-based): 2
Enter number of top recommendations: 3
```

#### Output:
```
Predicted ratings for unrated movies for User 2:
Movie 2: 2.65
Movie 3: 3.56

Top recommended movies for User 2 (Unrated Movies Only):
Movie 3 with predicted rating: 3.56
Movie 2 with predicted rating: 2.65

Top 3 recommended movies for User 2 (Including Both Rated and Unrated Movies):
Movie 1 with score: 4.00
Movie 3 with score: 3.56
Movie 2 with score: 2.65

RMSE for User 2: 2.65
```

## What to Expect
- **Predicted Ratings**: Displays predicted ratings for all unrated movies for the selected user.
- **Recommendations (Unrated Movies Only)**: Top `N` movie recommendations among unrated movies, based on predicted ratings.
- **Recommendations (All Movies)**: Top `N` movie recommendations including both rated and unrated movies.
- **RMSE**: The Root Mean Squared Error indicating the accuracy of the predictions for the given user.

## Edge Case Handling
- The program validates the ratings matrix to ensure it is non-empty.
- It checks for users with no ratings or movies with no ratings and handles these cases gracefully.

## License
This project is open-source and available under the [MIT License](LICENSE).
