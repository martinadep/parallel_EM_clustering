# ===============================
# Plot EM clustering results
# ===============================

data <- read.csv("./datasets/gmm_data_with_labels.csv")
em_results <- read.csv("./results/em_results.csv")

# Save as PNG
pdf("./results/em_clustering_comparison.pdf", width = 12, height = 4)
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
