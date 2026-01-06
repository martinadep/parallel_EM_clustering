# ===========================================
# Calculate EM clustering performance metrics
# ===========================================

library(gtools)

args <- commandArgs(trailingOnly = TRUE)

# Set default paths
true_data_path <- "./datasets/labels/gmm_P10000_K3_D2_labels.csv"
em_results_path <- "./results/em_P10000_K3_D2.csv"

if (length(args) >= 1) {
  true_data_path <- args[1]
}
if (length(args) >= 2) {
  em_results_path <- args[2]
}

cat("Loading data from:\n")
cat("  True labels:", true_data_path, "\n")
cat("  EM results:", em_results_path, "\n\n")

true_data <- read.csv(true_data_path)
em_results <- read.csv(em_results_path)

# Function to find the best label mapping using permutation search on a sample
find_best_mapping <- function(true_labels, pred_labels) {
  # Get unique labels
  unique_true <- sort(unique(true_labels))
  unique_pred <- sort(unique(pred_labels))
  
  # Create confusion matrix
  n_clusters <- length(unique_true)
  confusion <- matrix(0, nrow = n_clusters, ncol = n_clusters)
  
  for (i in 1:n_clusters) {
    for (j in 1:n_clusters) {
      confusion[i, j] <- sum(true_labels == unique_true[i] & pred_labels == unique_pred[j])
    }
  }
  
  # Find the best mapping by trying all permutations
  all_perms <- permutations(n_clusters, n_clusters)
  best_accuracy <- 0
  best_mapping <- NULL
  
  for (p in 1:nrow(all_perms)) {
    perm <- all_perms[p, ]
    accuracy <- sum(diag(confusion[, perm])) / sum(confusion)
    if (accuracy > best_accuracy) {
      best_accuracy <- accuracy
      best_mapping <- perm
    }
  }
  
  # Create mapping from predicted to true
  mapping <- setNames(unique_true, unique_pred[best_mapping])
  return(list(mapping = mapping, accuracy = best_accuracy))
}

# Calculate accuracy
cat("Comparing EM results with ground truth labels...\n")
cat("Number of data points:", nrow(true_data), "\n\n")

# Find optimal mapping using first 2000 points
sample_size <- min(2000, nrow(true_data))
cat("Finding optimal label mapping using first", sample_size, "points...\n")
result <- find_best_mapping(true_data$label[1:sample_size], em_results$label[1:sample_size])

cat("Optimal label mapping (EM -> True):\n")
print(result$mapping)
cat("\n")

# Apply mapping to predicted labels
mapped_labels <- result$mapping[as.character(em_results$label)]

# Calculate accuracy
accuracy <- mean(mapped_labels == true_data$label) * 100
cat("Clustering Accuracy:", sprintf("%.2f%%", accuracy), "\n")

# Calculate confusion matrix with mapped labels
confusion_matrix <- table(True = true_data$label, Predicted = mapped_labels)
cat("\nConfusion Matrix (after optimal mapping):\n")
print(confusion_matrix)

# Calculate per-class accuracy
cat("\nPer-class accuracy:\n")
for (label in sort(unique(true_data$label))) {
  class_accuracy <- confusion_matrix[as.character(label), as.character(label)] / sum(confusion_matrix[as.character(label), ]) * 100
  cat(sprintf("Class %d: %.2f%%\n", label, class_accuracy))
}


