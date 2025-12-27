# ===============================
# Gaussian Mixture Model Generator
# ===============================

set.seed(42)   # For reproducibility

# number of data points
N <- 1000

# Mixture weights
weights <- c(0.3, 0.4, 0.3)

# Means (each row is a mean vector)
means <- list(
  c(0, 0),
  c(8, 5),
  c(-5, 5)
)

# Covariance matrices
covariances <- list(
  matrix(c(1.0, 0.2,
           0.2, 1.0), nrow = 2),
  matrix(c(1.5, 0.0,
           0.0, 1.5), nrow = 2),
  matrix(c(1.0, -0.3,
           -0.3, 1.0), nrow = 2)
)

K <- length(weights)   # Number of components
D <- length(means[[1]]) # Dimension

# Sample component assignments
components <- sample(1:K, size = N, replace = TRUE, prob = weights)

# Function to sample from multivariate normal
rmvnorm <- function(n, mean, cov) {
  Z <- matrix(rnorm(n * length(mean)), ncol = length(mean))
  X <- Z %*% chol(cov)
  sweep(X, 2, mean, "+")
}

# Generate data
X <- matrix(0, nrow = N, ncol = D)

for (k in 1:K) {
  idx <- which(components == k)
  if (length(idx) > 0) {
    X[idx, ] <- rmvnorm(length(idx), means[[k]], covariances[[k]])
  }
}

# Convert to data frame
data <- data.frame(
  x1 = X[, 1],
  x2 = X[, 2]
)

# Save to CSV without labels
write.csv(data, "./datasets/gmm_data.csv", row.names = FALSE)
cat("Saved", N, "points to ./datasets/gmm_data.csv\n")

# plot(data$x1, data$x2, col = "blue",
#      main = "GMM Generated Data", xlab = "x1", ylab = "x2")

# Save to CSV with labels for reference
data$label <- components
write.csv(data, "./datasets/gmm_data_with_labels.csv", row.names = FALSE)
cat("Saved", N, "points to ./datasets/gmm_data_with_labels.csv\n")
