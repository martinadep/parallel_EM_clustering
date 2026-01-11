library(ggplot2)
library(dplyr)
library(readr)

# ==============================================================================
# CONFIGURAZIONE FILE
# ==============================================================================
# Percorsi relativi alla root del progetto
file_d6 <- "results/scaling_Ptest_D6_1.csv"
file_d7 <- "results/scaling_Ptest_D7_1.csv"
file_d8 <- "results/scaling_Ptest_D8_1.csv"

# ==============================================================================
# CARICAMENTO DATI
# ==============================================================================

read_and_label <- function(filepath, label) {
  if(!file.exists(filepath)) {
    # Fallback per compatibilità
    filepath_alt <- paste0("../", filepath)
    if(file.exists(filepath_alt)) {
      filepath <- filepath_alt
    } else {
      stop(paste("ERRORE: File non trovato ->", filepath))
    }
  }
  
  df <- read_csv(filepath, show_col_types = FALSE)
  df$Dimensions <- label
  df$Cores <- as.numeric(df$Cores)
  return(df)
}

cat("Caricamento dataset...\n")

df_d6 <- read_and_label(file_d6, "D=6")
df_d7 <- read_and_label(file_d7, "D=7")
df_d8 <- read_and_label(file_d8, "D=8")

# Unione
df_all <- bind_rows(df_d6, df_d7, df_d8)
df_all$Dimensions <- factor(df_all$Dimensions, levels = c("D=6", "D=7", "D=8"))

# Calcolo Metriche
df_metrics <- df_all %>%
  group_by(Dimensions) %>%
  arrange(Cores) %>%
  mutate(
    T1 = Time_Sec[Cores == 1],      # Tempo sequenziale
    Speedup = T1 / Time_Sec,        # Speedup
    Efficiency = Speedup / Cores    # Efficienza
  ) %>%
  ungroup()

# Calcolo massima efficienza per adattare il grafico (utile per super-linear scaling in D=8)
max_eff <- max(df_metrics$Efficiency, na.rm = TRUE)

# ==============================================================================
# GRAFICI (STILE UNIFORME)
# ==============================================================================

# Tema personalizzato
custom_theme <- theme_minimal() +
  theme(
    plot.title = element_text(face = "bold", size = 16, hjust = 0.5),
    plot.subtitle = element_text(size = 12, hjust = 0.5, color = "gray30"),
    axis.title = element_text(face = "bold", size = 12),
    legend.position = "bottom",
    panel.grid.minor = element_blank()
  )

# Palette colori alta visibilità
colors_d <- c("D=6" = "#1b9e77", "D=7" = "#d95f02", "D=8" = "#7570b3")  
breaks_cores <- sort(unique(df_metrics$Cores))

# --- 1. SPEEDUP ---
p_speedup <- ggplot(df_metrics, aes(x = Cores, y = Speedup, color = Dimensions, group = Dimensions)) +
  geom_abline(slope = 1, intercept = 0, linetype = "dashed", color = "gray60") +
  annotate("text", x = 8, y = 10, label = "Ideal Scaling", color = "gray60", angle = 45, vjust = -0.5) +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  scale_y_continuous(trans = "log2", breaks = breaks_cores) +
  scale_color_manual(values = colors_d) +
  labs(x = "Number of Cores (log scale)",
    y = "Speedup",
    color = "Dimensions"
  ) +
  custom_theme

# --- 2. EFFICIENCY ---
p_efficiency <- ggplot(df_metrics, aes(x = Cores, y = Efficiency, color = Dimensions, group = Dimensions)) +
  geom_hline(yintercept = 1, linetype = "dashed", color = "gray60") +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  # Limite Y dinamico
  scale_y_continuous(limits = c(0, max(1.2, max_eff * 1.05)), labels = scales::percent) +
  scale_color_manual(values = colors_d) +
  labs(x = "Number of Cores (log scale)",
    y = "Efficiency",
    color = "Dimensions"
  ) +
  custom_theme

# --- 3. TIME ---
p_time <- ggplot(df_metrics, aes(x = Cores, y = Time_Sec, color = Dimensions, group = Dimensions)) +
  geom_line(linewidth = 1) +
  geom_point(size = 3) +
  scale_x_continuous(trans = "log2", breaks = breaks_cores) +
  scale_y_continuous(trans = "log10") + # Logaritmico essenziale qui (differenze enormi)
  scale_color_manual(values = colors_d) +
  labs(x = "Number of Cores (log scale)",
    y = "Time in seconds (log scale)",
    color = "Dimensions"
  ) +
  custom_theme

# ==============================================================================
# SALVATAGGIO
# ==============================================================================

output_dir <- "results/plots"
if(!dir.exists("results") && dir.exists("../results")) output_dir <- "../results/plots"
dir.create(output_dir, showWarnings = FALSE)

cat(paste("Salvataggio grafici in", output_dir, "...\n"))

ggsave(file.path(output_dir, "scaling_D_speedup.png"), plot = p_speedup, width = 8, height = 6, dpi = 300)
ggsave(file.path(output_dir, "scaling_D_efficiency.png"), plot = p_efficiency, width = 8, height = 6, dpi = 300)
ggsave(file.path(output_dir, "scaling_D_time.png"), plot = p_time, width = 8, height = 6, dpi = 300)
cat("Grafici salvati in", output_dir, "\n")