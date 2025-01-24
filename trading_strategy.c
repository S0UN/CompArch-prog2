#include <stdio.h>
#include <stdlib.h>

double capital = 10000.0;
double invested_capital = 0.0;
double investment_basis = 0;
double last_sale_price = 0;
double purchase_price = 0; 
int invested = 1;
int count_rows(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file.\n");
        return -1;
    }

    int lines = 0;
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), file))
    {
        lines++;
    }

    fclose(file);
    return lines - 1;
}

int main()
{
    const char *filename = "SPX.csv";

    int num_rows = count_rows(filename);
    if (num_rows <= 0)
    {
        printf("No data to process.\n");
        return 1;
    }

    printf("Number of rows: %d\n", num_rows);

    double *prices = malloc(num_rows * sizeof(double));
    if (prices == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file.\n");
        free(prices);
        return 1;
    }

    char line[100];
    fgets(line, sizeof(line), file);

    int count = 0;
    while (fgets(line, sizeof(line), file))
    {
        double close_price;
        sscanf(line, "%*[^,],%lf", &close_price);
        prices[count++] = close_price;
    }

    fclose(file);

    printf("Closing Prices:\n");
    for (int i = 0; i < num_rows; i++)
    {
        printf("Day %d: %.2f\n", i + 1, prices[i]);
    }
    purchase_price = prices[0];

    for (int i = 0; i < num_rows; i++)
    {
        if (invested == 1)
        {
            // Update investment basis if price rises
            if (prices[i] > investment_basis)
            {
                investment_basis = prices[i];
            }

            // Sell if price drops 2% below investment basis
            if (prices[i] < investment_basis * 0.98)
            {
                capital += (invested_capital / purchase_price) * prices[i];
                invested_capital = 0.0; // No longer invested
                last_sale_price = prices[i];
                invested = 0; // No longer invested
            }
        }
        else
        {
            // Buy if price rises 2% above the last sale price
            if (prices[i] > last_sale_price * 1.02)
            {
                invested_capital = capital; // Use all available capital
                investment_basis = prices[i];
                purchase_price = prices[i];
                capital = 0.0; // All cash is now invested
                invested = 1;
            }

            // Update last sale price if price drops further
            if (prices[i] < last_sale_price)
            {
                last_sale_price = prices[i];
            }
        }
    }

    // Final adjustment: Sell remaining stock
    if (invested)
    {
        capital += (invested_capital / purchase_price) * prices[num_rows - 1];
        invested_capital = 0.0;
        invested = 0;
    }

    printf("Final Capital: %.2f\n", capital);

    free(prices);

    return 0;
}
