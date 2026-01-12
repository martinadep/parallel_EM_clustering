# ===============================
# Plot EM clustering results
# ===============================

args <- commandArgs(trailingOnly = TRUE)

# Set default paths
true_data_path <- "./datasets/labels/gmm_P10000_K3_D2_labels.csv"
em_results_path <- "./results/em_P10000_K3_D2.csv"
output_path <- "./results/plots/em_plots_P10000_K3_D2.pdf"

if (length(args) >= 1) {
  true_data_path <- args[1]
}
if (length(args) >= 2) {
  em_results_path <- args[2]
}
if (length(args) >= 3) {
  output_path <- args[3]
}

cat("Loading data from:\n")
cat("  True labels:", true_data_path, "\n")
cat("  EM results:", em_results_path, "\n")
cat("  Output file:", output_path, "\n\n")

data <- read.csv(true_data_path)
em_results <- read.csv(em_results_path)

# Save as PDF
pdf(output_path, width = 12, height = 4)
# png("./results/em_clustering_comparison.png", width = 1200, height = 400)

# Create a single figure with 3 subplots
par(mfrow = c(1, 3), pty = "s", mar = c(4, 4, 2, 1))
# Left: Data only
plot(data$x1, data$x2, main = "Data", xlab = "x1", ylab = "x2",
     xlim = c(min(data$x1, em_results$x1), max(data$x1, em_results$x1)),
     ylim = c(min(data$x2, em_results$x2), max(data$x2, em_results$x2)))

# Center: Data with true labels
plot(data$x1, data$x2, col = data$label,
     main = "True Labels", xlab = "x1", ylab = "x2",
     xlim = c(min(data$x1, em_results$x1), max(data$x1, em_results$x1)),
     ylim = c(min(data$x2, em_results$x2), max(data$x2, em_results$x2)))

# Right: EM clustering results
plot(em_results$x1, em_results$x2, col = em_results$label,
     main = "EM Results", xlab = "x1", ylab = "x2",
     xlim = c(min(data$x1, em_results$x1), max(data$x1, em_results$x1)),
     ylim = c(min(data$x2, em_results$x2), max(data$x2, em_results$x2)))

# Reset layout
par(mfrow = c(1, 1))
dev.off()
