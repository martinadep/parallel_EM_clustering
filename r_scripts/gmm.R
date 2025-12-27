# Install and load the 'mclust' package
library(mclust)

# Generate a synthetic dataset with three clusters
set.seed(123)
data <- rbind(matrix(rnorm(100, mean = 0, sd = 1), ncol = 2),
              matrix(rnorm(100, mean = 5, sd = 1), ncol = 2),
              matrix(rnorm(100, mean = 10, sd = 1), ncol = 2))

# Perform GMM clustering
# G represents the number of clusters
gmm_model <- Mclust(data, G = 3)  

# Get the cluster assignments
cluster_assignments <- predict(gmm_model)$classification

# Visualize the results
plot(data, col = cluster_assignments, main = "GMM Clustering Results")
points(gmm_model$parameters$mean, col = 1:3, pch = 8, cex = 2)

# ESTIMATE PROBABILITIES
target_vectors <- matrix(c(0, 0, 2, 2, 5, 5), nrow = 3, byrow = TRUE)
density_estimates <- predict(gmm_model, newdata = target_vectors, what = "density")
print(density_estimates)

cat("--- Cluster Means Vector ---\n")
print(gmm_model$parameters$mean)

cat("\n--- Cluster Covariance Matrices ---\n")
print(gmm_model$parameters$variance$sigma)
