library(ggplot2)
library(dplyr)
library(readr)
library(gridExtra)

# --- CONFIGURAZIONE ---
# Inserisci qui i percorsi ai tuoi file CSV
files <- list(
  "200k" = "../results/scaling_Ptest_200k_1.csv",
  "300k" = "../results/scaling_Ptest_300k_1.csv",
  "500k" = "../results/scaling_Ptest_500k_1.csv"
)

# --- CARICAMENTO E ELABORAZIONE DATI ---
data_list <- list()

for (label in names(files)) {
  if(file.exists(files[[label]])) {
    df <- read_csv(files[[label]], col_types = cols())
    
    # Prendi il tempo sequenziale (1 core) come baseline
    t_serial <- df %>% filter(Cores == 1) %>% pull(Time_Sec)
    
    # Calcola Speedup e Efficiency
    df <- df %>%
      mutate(
        Dataset = label,
        Speedup = t_serial / Time_Sec,
        Efficiency = Speedup / Cores,
        Ideal_Speedup = Cores
      )
    data_list[[label]] <- df
  } else {
    warning(paste("File non trovato:", files[[label]]))
  }
}

all_data <- bind_rows(data_list)
all_data$Dataset <- factor(all_data$Dataset, levels = c("200k", "300k", "500k"))

# --- CREAZIONE GRAFICO SPEEDUP ---
p_speedup <- ggplot(all_data, aes(x = Cores, y = Speedup, group = Dataset, color = Dataset)) +
  # Linea ideale
  geom_abline(slope = 1, intercept = 0, linetype = "dashed", color = "gray50") +
  annotate("text", x = 10, y = 12, label = "Ideal Scaling", angle = 45, color = "gray50", size = 3) +
  # Linee dati - CORRETTO: linewidth invece di size
  geom_line(linewidth = 1) +
  geom_point(size = 2) +
  scale_x_continuous(trans = "log2", breaks = c(1, 2, 4, 8, 16, 32, 64)) +
  # CORRETTO: trans invece di proc
  scale_y_continuous(trans = "log2", breaks = c(1, 2, 4, 8, 16, 32, 64)) +
  labs(title = "Strong Scaling Analysis (Variable N)",
       subtitle = "Speedup vs Cores (Log-Log Scale)",
       x = "Number of Cores",
       y = "Speedup (T1 / Tn)") +
  theme_bw() +
  theme(legend.position = "bottom")

# --- CREAZIONE GRAFICO EFFICIENZA ---
p_efficiency <- ggplot(all_data, aes(x = Cores, y = Efficiency, group = Dataset, color = Dataset)) +
  geom_hline(yintercept = 1, linetype = "dashed", color = "gray50") +
  geom_line(linewidth = 1) +
  geom_point(size = 2) +
  scale_x_continuous(trans = "log2", breaks = c(1, 2, 4, 8, 16, 32, 64)) +
  scale_y_continuous(limits = c(0, 1.1), labels = scales::percent) +
  labs(title = "Parallel Efficiency",
       subtitle = "Energy Efficiency vs Cores",
       x = "Number of Cores",
       y = "Efficiency (Speedup / Cores)") +
  theme_bw() +
  theme(legend.position = "bottom")

# --- SALVATAGGIO ---
# Salva combinato in un PDF (vettoriale, perfetto per LaTeX)
pdf("../results/scaling_N_plot.pdf", width = 10, height = 5)
grid.arrange(p_speedup, p_efficiency, ncol = 2)
dev.off()

print("Grafico salvato in: results/scaling_N_plot.pdf")