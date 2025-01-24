#include <stdio.h>
#include <stdlib.h>

// Global variables
double capital = 0.0;
double invested_capital = 10000.0;
double investment_basis = 0;
double last_sale_price = 0;
double purchase_price = 0;
int invested = 1;

// Function to count the number of rows in the file
int count_rows(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return -1;
    }

    int lines = 0;
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), file)) {
        lines++;
    }

    fclose(file);
    return lines - 1;
}

// Function to read prices from the file
double *read_prices(const char *filename, int num_rows) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return NULL;
    }

    double *prices = malloc(num_rows * sizeof(double));
    if (prices == NULL) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    char line[100];
    // Skip the header
    fgets(line, sizeof(line), file); 

    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        double close_price;
        sscanf(line, "%*[^,],%lf", &close_price);
        prices[count++] = close_price;
    }

    fclose(file);
    return prices;
}

// Function to handle investment logic when invested
void handle_invested(double price) {
    if (price > investment_basis) {
        investment_basis = price;
    }

    if (price < investment_basis * 0.98) {
        capital += (invested_capital / purchase_price) * price;
        invested_capital = 0.0;
        last_sale_price = price;
        invested = 0;
    }
}

// Function to handle investment logic when not invested
void handle_not_invested(double price) {
    if (price > last_sale_price * 1.02) {
        invested_capital = capital;
        investment_basis = price;
        purchase_price = price;
        capital = 0.0;
        invested = 1;
    }

    if (price < last_sale_price) {
        last_sale_price = price;
    }
}

// Function to sell remaining stock at the end of simulation
void sell_remaining_stock(double final_price) {
    if (invested) {
        capital += (invested_capital / purchase_price) * final_price;
        invested_capital = 0.0;
        invested = 0;
    }
}

// Main simulation logic
void run_simulation(const double *prices, int num_rows) {
    purchase_price = prices[0];

    for (int i = 0; i < num_rows; i++) {
        if (invested == 1) {
            handle_invested(prices[i]);
        } else {
            handle_not_invested(prices[i]);
        }

        if (capital != 0) {
            printf("Day %d: %.2f\n", i + 1, capital);
        } else {
            printf("Day %d: %.2f\n", i + 1, invested_capital);
        }
    }

    sell_remaining_stock(prices[num_rows - 1]);
    printf("Ending Capital: %.2f\n", capital);
}

int main() {
    const char *filename = "SPX.csv";

    int num_rows = count_rows(filename);
    if (num_rows <= 0) {
        printf("No data to process.\n");
        return 1;
    }

    printf("Number of rows: %d\n", num_rows);

    double *prices = read_prices(filename, num_rows);
    if (prices == NULL) {
        return 1;
    }

    run_simulation(prices, num_rows);

    free(prices);
    return 0;
}
