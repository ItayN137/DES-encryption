import com.sun.jna.Library;
import com.sun.jna.Native;
import javax.swing.*;
import java.awt.*;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.Random;

public class UserInterface extends JFrame {
    private JTextArea inputText, outputText;
    private JTextField keyField;
    private JButton encryptButton, decryptButton, copyToClipboardButton, generateKeyButton;
    private byte[] fileData;

    // Define the interface for JNA
    public interface DESAlgorithm extends Library {
        DESAlgorithm INSTANCE = (DESAlgorithm) Native.loadLibrary("DESAlgorithm", DESAlgorithm.class);

        String DESAlgorithm(String msg, String key, int mode);
    }

    static {
        // Load the DLL file
        try {
            System.load("C:\\Users\\itayn\\IdeaProjects\\DES_Algorithm\\src\\DESAlgorithm.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.");
            e.printStackTrace();
            System.exit(1);
        }
    }

    public UserInterface() {
        setTitle("DES Encryption/Decryption");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1280, 720);
        setLocationRelativeTo(null);
        setResizable(false);

        // Initialize components
        inputText = new JTextArea(10, 20);
        outputText = new JTextArea(10, 20);
        keyField = new JTextField(40); // Enlarged key text box

        outputText.setEditable(false);

        inputText.setBackground(Color.DARK_GRAY);
        inputText.setForeground(Color.WHITE);
        inputText.setCaretColor(Color.WHITE);
        inputText.setFont(new Font("Arial", Font.PLAIN, 18));

        outputText.setBackground(Color.DARK_GRAY);
        outputText.setForeground(Color.WHITE);
        outputText.setCaretColor(Color.WHITE);
        outputText.setFont(new Font("Arial", Font.PLAIN, 18));

        keyField.setBackground(Color.WHITE); // Set key text field background to white
        keyField.setFont(new Font("Arial", Font.PLAIN, 18)); // Set font for key text field
        keyField.setPreferredSize(new Dimension(200, 30)); // Enlarge key text field
        keyField.setEditable(true); // Enable manual editing of key field
        keyField.setForeground(Color.BLACK); // Set text color to black

        encryptButton = new JButton("Encrypt");
        encryptButton.setBackground(new Color(23, 162, 184));
        encryptButton.setForeground(Color.WHITE);
        encryptButton.setFocusPainted(false);
        encryptButton.setFont(new Font("Arial", Font.BOLD, 16));
        encryptButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                new Thread(() -> {
                    try {
                        System.out.println("Encryption started...");
                        String msg = inputText.getText();
                        String key = keyField.getText();
                        System.out.println("Message: " + msg);
                        System.out.println("Key: " + key);
                        String encryptedText = DESAlgorithm.INSTANCE.DESAlgorithm(msg, key, 1);
                        SwingUtilities.invokeLater(() -> outputText.setText(encryptedText));
                        System.out.println("Encryption completed.");
                    } catch (Exception ex) {
                        ex.printStackTrace();
                        JOptionPane.showMessageDialog(UserInterface.this, "Encryption failed: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
                    }
                }).start();
            }
        });

        decryptButton = new JButton("Decrypt");
        decryptButton.setBackground(new Color(23, 162, 184));
        decryptButton.setForeground(Color.WHITE);
        decryptButton.setFocusPainted(false);
        decryptButton.setFont(new Font("Arial", Font.BOLD, 16));
        decryptButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                new Thread(() -> {
                    try {
                        System.out.println("Decryption started...");
                        String msg = inputText.getText();
                        String key = keyField.getText();
                        System.out.println("Message: " + msg);
                        System.out.println("Key: " + key);
                        String decryptedText = DESAlgorithm.INSTANCE.DESAlgorithm(msg, key, 0);
                        SwingUtilities.invokeLater(() -> outputText.setText(decryptedText));
                        System.out.println("Decryption completed.");
                    } catch (Exception ex) {
                        ex.printStackTrace();
                        JOptionPane.showMessageDialog(UserInterface.this, "Decryption failed: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
                    }
                }).start();
            }
        });

        copyToClipboardButton = new JButton("Copy to Clipboard");
        copyToClipboardButton.setBackground(new Color(23, 162, 184));
        copyToClipboardButton.setForeground(Color.WHITE);
        copyToClipboardButton.setFocusPainted(false);
        copyToClipboardButton.setFont(new Font("Arial", Font.BOLD, 16));
        copyToClipboardButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                StringSelection selection = new StringSelection(outputText.getText());
                Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
                clipboard.setContents(selection, null);
            }
        });

        generateKeyButton = new JButton("Generate Key");
        generateKeyButton.setBackground(new Color(23, 162, 184));
        generateKeyButton.setForeground(Color.WHITE);
        generateKeyButton.setFocusPainted(false);
        generateKeyButton.setFont(new Font("Arial", Font.BOLD, 16));
        generateKeyButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                generateRandomKey();
            }
        });

        // Menu Bar
        JMenuBar menuBar = new JMenuBar();
        JMenu fileMenu = new JMenu("File");
        JMenuItem openFileItem = new JMenuItem("Open File");

        openFileItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                openFile();
            }
        });

        fileMenu.add(openFileItem);
        menuBar.add(fileMenu);
        setJMenuBar(menuBar);

        // Layout
        JPanel inputPanel = new JPanel(new BorderLayout());
        JLabel inputLabel = new JLabel("Input:", SwingConstants.LEFT);
        inputLabel.setFont(new Font("Arial", Font.BOLD, 20));
        inputPanel.add(inputLabel, BorderLayout.NORTH);
        inputPanel.add(new JScrollPane(inputText), BorderLayout.CENTER);

        JPanel outputPanel = new JPanel(new BorderLayout());
        JLabel outputLabel = new JLabel("Output:", SwingConstants.LEFT);
        outputLabel.setFont(new Font("Arial", Font.BOLD, 20));
        outputPanel.add(outputLabel, BorderLayout.NORTH);
        outputPanel.add(new JScrollPane(outputText), BorderLayout.CENTER);

        JPanel keyPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 20, 10));
        keyPanel.setBackground(Color.DARK_GRAY);
        JLabel keyLabel = new JLabel("Key:");
        keyLabel.setFont(new Font("Arial", Font.BOLD, 16));
        keyLabel.setForeground(Color.WHITE); // Set key label text color to white
        keyPanel.add(keyLabel);
        keyPanel.add(keyField);

        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 20, 10));
        buttonPanel.setBackground(Color.DARK_GRAY);
        buttonPanel.add(encryptButton);
        buttonPanel.add(decryptButton);
        buttonPanel.add(copyToClipboardButton);
        buttonPanel.add(generateKeyButton); // Add generate key button to button panel

        JPanel mainPanel = new JPanel(new BorderLayout(10, 10));
        mainPanel.setBackground(Color.DARK_GRAY);
        mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        mainPanel.add(inputPanel, BorderLayout.NORTH);
        mainPanel.add(outputPanel, BorderLayout.CENTER);
        mainPanel.add(keyPanel, BorderLayout.WEST);
        mainPanel.add(buttonPanel, BorderLayout.SOUTH);

        add(mainPanel);

        setVisible(true);
    }

    private void openFile() {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Open File");
        int option = fileChooser.showOpenDialog(this);
        if (option == JFileChooser.APPROVE_OPTION) {
            File file = fileChooser.getSelectedFile();
            readFileInBackground(file);
        }
    }

    private void readFileInBackground(File file) {
        new Thread(() -> {
            try {
                fileData = readBytesFromFile(file);
                String hexContent = bytesToHex(fileData);
                SwingUtilities.invokeLater(() -> inputText.setText(hexContent));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    private byte[] readBytesFromFile(File file) throws IOException {
        try (FileInputStream fis = new FileInputStream(file);
             BufferedInputStream bis = new BufferedInputStream(fis)) {
            byte[] bytes = new byte[(int) file.length()];
            int bytesRead = bis.read(bytes);
            if (bytesRead < bytes.length) {
                throw new IOException("Could not read entire file");
            }
            return bytes;
        }
    }

    private String bytesToHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        for (byte b : bytes) {
            sb.append(String.format("%02X", b));
        }
        return sb.toString();
    }

    private void generateRandomKey() {
        Random random = new Random();
        char[] keyChars = new char[8]; // 8 characters key for DES
        for (int i = 0; i < 8; i++) {
            // Generate a random ASCII character between 33 and 126 (inclusive)
            keyChars[i] = (char) (random.nextInt(94) + 33);
        }
        keyField.setText(new String(keyChars));
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(UserInterface::new);
    }
}
