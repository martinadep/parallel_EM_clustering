library(ggplot2)
library(dplyr)
library(readr)

# ==============================================================================
# CONFIGURAZIONE FILE
# ==============================================================================
file_k5  <- "results/scaling_Ptest_K5_1.csv"
file_k10 <- "results/scaling_Ptest_K10_2.csv"
file_k15 <- "results/scaling_Ptest_K15_1.csv"

# ==============================================================================
# CARICAMENTO DATI
# ==============================================================================
read_and_label <- function(filepath, k_label) {
  if(!file.exists(filepath)) stop(paste("ERRORE: File non trovato ->", filepath))
  df <- read_csv(filepath, show_col_types = FALSE)
  df$K_Label <- k_label
  df$Cores <- as.numeric(df$Cores)
  return(df)
}

cat("Caricamento dataset...\n")
df_k5  <- read_and_label(file_k5, "K=5")
df_k10 <- read_and_label(file_k10, "K=10")
df_k15 <- read_and_label(file_k15, "K=15")

df_all <- bind_rows(df_k5, df_k10, df_k15)
df_all$K_Label <- factor(df_all$K_Label, levels = c("K=5", "K=10", "K=15"))

# Calcolo Metriche
df_metrics <- df_all %>%
  group_by(K_Label) %>%
  arrange(Cores) %>%
  mutate(
    T1 = Time_Sec[Cores == 1],      # Tempo sequenziale
    Speedup = T1 / Time_Sec,        # Speedup
    Efficiency = Speedup / Cores    # Efficienza
  ) %>%
  ungroup()

# Controllo se ci sono valori di efficienza molto alti
max_eff <- max(df_metrics$Efficiency, na.rm = TRUE)
cat("Massima efficienza rilevata:", round(max_eff, 2), "\n")

# ==============================================================================
# GRAFICI
# ==============================================================================
custom_theme <- theme_minimal() +
  theme(
    plot.title = element_text(face = "bold", size = 16, hjust = 0.5),
    plot.subtitle = element_text(size = 12, hjust = 0.5, color = "gray30"),
    legend.position = "bottom",
    panel.grid.minor = element_blank()
  )

colors_k <- c("K=5" = "#E69F00", "K=10" = "#56B4E9", "K=15" = "#009E73")
breaks_cores <- sort(unique(df_metrics$Cores))

# --- 1. SPEEDUP ---
p_speedup <- ggplot(df_metrics, aes(x = Cores, y = Speedup, color = K_Label, group = K_Label)) +
  geom_abline(slope = 1, intercept = 0, linetype = "dashed", color = "gray60") +
  # Linee e punti
  geom_line(linewidth = 1) + 
  geom_point(size = 3) +
  # Assi
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  scale_y_continuous(trans = "log2", breaks = breaks_cores) +
  scale_color_manual(values = colors_k) +
  labs(x = "Cores", y = "Speedup", color = "Clusters") +
  custom_theme

# --- 2. EFFICIENCY ---
p_efficiency <- ggplot(df_metrics, aes(x = Cores, y = Efficiency, color = K_Label, group = K_Label)) +
  geom_hline(yintercept = 1, linetype = "dashed", color = "gray60") +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  # MODIFICATO: Aggiunto scale::percent
  scale_y_continuous(limits = c(0, max(1.2, max_eff * 1.05)), labels = scales::percent) + 
  scale_color_manual(values = colors_k) +
  labs(x = "Cores", y = "Efficiency", color = "Clusters") +
  custom_theme

# --- 3. TIME ---
p_time <- ggplot(df_metrics, aes(x = Cores, y = Time_Sec, color = K_Label, group = K_Label)) +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  scale_y_continuous(trans = "log10") +
  scale_color_manual(values = colors_k) +
  labs(x = "Cores", y = "Time (s)", color = "Clusters") +
  custom_theme

# ==============================================================================
# SALVATAGGIO
# ==============================================================================
dir.create("results/plots", showWarnings = FALSE)

ggsave("results/plots/scaling_K_speedup.png", plot = p_speedup, width = 8, height = 6, dpi = 300)
ggsave("results/plots/scaling_K_efficiency.png", plot = p_efficiency, width = 8, height = 6, dpi = 300)
ggsave("results/plots/scaling_K_time.png", plot = p_time, width = 8, height = 6, dpi = 300)

cat("Grafici salvati in results/plots/\n")