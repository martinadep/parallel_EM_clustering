library(ggplot2)
library(dplyr)
library(readr)

# ==============================================================================
# CONFIGURAZIONE FILE
# ==============================================================================
# Percorsi relativi alla root del progetto
file_200k <- "results/omp/scaling_Ptest_200k_1.csv"
file_300k <- "results/omp/scaling_Ptest_300k_1.csv"
file_500k <- "results/omp/scaling_Ptest_500k_1.csv"

# ==============================================================================
# CARICAMENTO DATI
# ==============================================================================

read_and_label <- function(filepath, label) {
  if(!file.exists(filepath)) {
    # Fallback per compatibilità se lanciato dalla cartella r_scripts invece che dalla root
    filepath_alt <- paste0("../", filepath)
    if(file.exists(filepath_alt)) {
      filepath <- filepath_alt
    } else {
      stop(paste("ERRORE: File non trovato ->", filepath))
    }
  }
  
  df <- read_csv(filepath, show_col_types = FALSE)
  df$Dataset <- label
  df$Cores <- as.numeric(df$Cores)
  return(df)
}

cat("Caricamento dataset...\n")

df_200 <- read_and_label(file_200k, "N=200k")
df_300 <- read_and_label(file_300k, "N=300k")
df_500 <- read_and_label(file_500k, "N=500k")

# Unione
df_all <- bind_rows(df_200, df_300, df_500)
df_all$Dataset <- factor(df_all$Dataset, levels = c("N=200k", "N=300k", "N=500k"))

# Calcolo Metriche per ogni dataset
df_metrics <- df_all %>%
  group_by(Dataset) %>%
  arrange(Cores) %>%
  mutate(
    T1 = Time_Sec[Cores == 1],      # Tempo sequenziale
    Speedup = T1 / Time_Sec,        # Speedup
    Efficiency = Speedup / Cores    # Efficienza
  ) %>%
  ungroup()

# Calcolo massima efficienza per adattare il grafico
max_eff <- max(df_metrics$Efficiency, na.rm = TRUE)

# ==============================================================================
# GRAFICI (STILE UNIFORME)
# ==============================================================================

# Tema personalizzato (lo stesso usato per K)
custom_theme <- theme_minimal() +
  theme(
    plot.title = element_text(face = "bold", size = 16, hjust = 0.5),
    plot.subtitle = element_text(size = 12, hjust = 0.5, color = "gray30"),
    axis.title = element_text(face = "bold", size = 12),
    legend.position = "bottom",
    panel.grid.minor = element_blank()
  )

# Palette colori per N (Blu scuro, Blu medio, Rosso scuro o simili per distinguere)
colors_n <- c("N=200k" = "#D55E00", "N=300k" = "#0072B2", "N=500k" = "#CC79A7")
breaks_cores <- sort(unique(df_metrics$Cores))

# --- 1. SPEEDUP ---
p_speedup <- ggplot(df_metrics, aes(x = Cores, y = Speedup, color = Dataset, group = Dataset)) +
  geom_abline(slope = 1, intercept = 0, linetype = "dashed", color = "gray60") +
  annotate("text", x = 8, y = 10, label = "Ideal Scaling", color = "gray60", angle = 45, vjust = -0.5) +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  scale_y_continuous(trans = "log2", breaks = breaks_cores) +
  scale_color_manual(values = colors_n) +
  labs(x = "Number of Cores (log scale)",
    y = "Speedup",
    color = "Dataset"
  ) +
  custom_theme

# --- 2. EFFICIENCY ---
p_efficiency <- ggplot(df_metrics, aes(x = Cores, y = Efficiency, color = Dataset, group = Dataset)) +
  geom_hline(yintercept = 1, linetype = "dashed", color = "gray60") +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  # Limite Y dinamico per accomodare efficienze > 1
  scale_y_continuous(limits = c(0, max(1.2, max_eff * 1.05)), labels = scales::percent) +
  scale_color_manual(values = colors_n) +
  labs(x = "Number of Cores (log scale)",
    y = "Efficiency",
    color = "Dataset"
  ) +
  custom_theme

# --- 3. TIME ---
p_time <- ggplot(df_metrics, aes(x = Cores, y = Time_Sec, color = Dataset, group = Dataset)) +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  scale_y_continuous(trans = "log10") + # Logaritmico per i tempi
  scale_color_manual(values = colors_n) +
  labs(x = "Number of Cores (log scale)",
    y = "Time in seconds (log scale)",
    color = "Dataset"
  ) +
  custom_theme


# ==============================================================================
# SALVATAGGIO
# ==============================================================================
dir.create("results/plots/omp", showWarnings = FALSE)

ggsave("results/plots/omp/scaling_N_speedup.png", plot = p_speedup, width = 8, height = 6, dpi = 300)
ggsave("results/plots/omp/scaling_N_efficiency.png", plot = p_efficiency, width = 8, height = 6, dpi = 300)
ggsave("results/plots/omp/scaling_N_time.png", plot = p_time, width = 8, height = 6, dpi = 300)

cat("Grafici salvati in results/plots/omp/\n")