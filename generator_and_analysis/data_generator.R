# ===============================
# Gaussian Mixture Model Generator
# ===============================

set.seed(41) 

args <- commandArgs(trailingOnly = TRUE)

# Set default values
N <- 10000  # number of data points
K <- 3       # number of clusters
D <- 2       # dimensions

if (length(args) >= 1) {
  N <- as.integer(args[1])
}
if (length(args) >= 2) {
  K <- as.integer(args[2])
}
if (length(args) >= 3) {
  D <- as.integer(args[3])
}

cat("Generating GMM data with:\n")
cat("  N (points):", N, "\n")
cat("  K (clusters):", K, "\n")
cat("  D (dimensions):", D, "\n\n")

# Generate random weights that sum to 1
weights <- runif(K)
weights <- weights / sum(weights)

# Generate random means
means <- list()
space_scale <- 15

for (k in 1:K) {
  means[[k]] <- runif(D, min = -space_scale, max = space_scale)
}

# Generate random covariances
covariances <- list()

for (k in 1:K) {
  # base spread per cluster
  base_var <- runif(1, 0.5, 4)   # piccola-varianza a grande-varianza

  # piccola differenza tra le dimensioni
  vars <- base_var * runif(D, 0.8, 1.2)

  Sigma <- diag(vars)

  # aggiungi leggera correlazione (solo se D > 1)
  if (D > 1) {
    rho <- runif(1, -0.4, 0.4)
    Sigma[1,2] <- rho * sqrt(vars[1] * vars[2])
    Sigma[2,1] <- Sigma[1,2]
  }

  covariances[[k]] <- Sigma
}

base_name <- sprintf("gmm_P%d_K%d_D%d", N, K, D)

# Print cluster parameters table
cat("\nCluster Parameters:\n")
cat(strrep("-", 80), "\n")
cat(sprintf("%-8s %-12s %s\n", "Cluster", "Weight", "Mean"))
cat(strrep("-", 80), "\n")
for (k in 1:K) {
  mean_str <- paste("[", paste(sprintf("%.3f", means[[k]]), collapse=", "), "]", sep="")
  cat(sprintf("%-8d %-12.3f %s\n", k-1, weights[k], mean_str))
}
cat(strrep("-", 80), "\n\n")

# Create datasets directory if it doesn't exist
if (!dir.exists("./datasets")) {
  dir.create("./datasets", recursive = TRUE)
}
if (!dir.exists("./datasets/labels")) {
  dir.create("./datasets/labels", recursive = TRUE)
}

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

# Convert to data frame with dynamic column names
data <- as.data.frame(X)
if (D == 2) {
  colnames(data) <- c("x1", "x2")
} else {
  colnames(data) <- paste0("x", 1:D)
}

# Save to CSV without labels
unlabeled_path <- file.path("./datasets", paste0(base_name, ".csv"))
write.csv(data, unlabeled_path, row.names = FALSE)
cat("Saved", N, "points to", unlabeled_path, "\n")

# plot(data$x1, data$x2, col = "blue",
#      main = "GMM Generated Data", xlab = "x1", ylab = "x2")

# Save to CSV with labels for reference
data$label <- components
labels_path <- file.path("./datasets/labels", paste0(base_name, "_labels.csv"))
write.csv(data, labels_path, row.names = FALSE)
cat("Saved", N, "points to", labels_path, "\n")

plot(data$x1, data$x2, col = data$label,
     main = "GMM Generated Data with True Labels", xlab = "x1", ylab = "x2")


