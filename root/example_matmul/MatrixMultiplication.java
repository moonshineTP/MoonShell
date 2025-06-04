package root.example_matmul;

import java.io.File;
import java.util.Scanner;
import java.io.PrintWriter;

public class MatrixMultiplication {
    public static void main (String[] args) {
        if (args.length != 3) {
            throw new IllegalArgumentException("Wrong number of arguments");
        }
        
        String mat1Path = args[0];
        String mat2Path = args[1];
        String mat3Path = args[2];

        int[][] mat1 = loadMatrix(mat1Path);
        int[][] mat2 = loadMatrix(mat2Path);

        int[][] mat3 = mulMatrix(mat1, mat2);
        writeMatrix(mat3, mat3Path);
    }

    public static int[][] loadMatrix(String path) {
        File file = new File(path);
        if (!file.exists()) {
            throw new IllegalArgumentException("File not found: " + path);
        }

        try (Scanner scanner = new Scanner(file)) {
            // Read the first line for row and col
            String[] dims = scanner.nextLine().split(",");
            int row = Integer.parseInt(dims[0].trim());
            int col = Integer.parseInt(dims[1].trim());

            int[][] mat = new int[row][col];
            for (int i = 0; i < row; i++) {
                if (!scanner.hasNextLine()) {
                    throw new IllegalArgumentException("Not enough rows in matrix file: " + path);
                }
                String[] values = scanner.nextLine().split(",");
                if (values.length != col) {
                    throw new IllegalArgumentException("Column count mismatch at row " + (i + 1));
                }
                for (int j = 0; j < col; j++) {
                    mat[i][j] = Integer.parseInt(values[j]);
                }
            }
            return mat;
        } catch (Exception e) {
            throw new RuntimeException("Failed to load matrix from file: " + path, e);
        }
    }

    public static int[][] mulMatrix(int[][] mat1, int[][] mat2) {
        int rows1 = mat1.length;
        int cols1 = mat1[0].length;
        int rows2 = mat2.length;
        int cols2 = mat2[0].length;

        if (cols1 != rows2) {
            throw new IllegalArgumentException("Matrix dimensions do not match for multiplication.");
        }

        int[][] result = new int[rows1][cols2];
        for (int i = 0; i < rows1; i++) {
            for (int j = 0; j < cols2; j++) {
                int sum = 0;
                for (int k = 0; k < cols1; k++) {
                    sum += mat1[i][k] * mat2[k][j];
                }
                result[i][j] = sum;
            }
        }
        return result;
    }

    public static void writeMatrix(int[][] mat, String path) {
        try (PrintWriter writer = new PrintWriter(path)) {
            int rows = mat.length;
            int cols = mat[0].length;
            writer.println(rows + "," + cols);
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    writer.print(mat[i][j]);
                    if (j < cols - 1) {
                        writer.print(",");
                    }
                }
                writer.println();
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed to write matrix to file: " + path, e);
        }
    }
}