package hw3;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;

import java.math.*;

import java.awt.event.*;

class OperatorNode {
	char item;
	OperatorNode next;
}

class NumberNode{
	double item;
	NumberNode next;
}

class OperatorStack{
	OperatorNode top;
	
	public OperatorStack(){
		this.top = null;
	}
	
	public boolean isEmpty() {
		if (top == null)
			return true;
		else return false;
	}
	
	public void push(char item) {
		OperatorNode node = new OperatorNode();
		node.item = item;
		node.next = top;
		top = node;
	}
	
	public char pop() {
		if(isEmpty() == true)
			return 0;
		else {
			OperatorNode node = top;
			top = node.next;
			return node.item;
		}
	}
	
	public char peek() {
		if(isEmpty() == true)
			return 0;
		else {
			return top.item;
		}
	}
}

class NumberStack{
	NumberNode top;
	
	public NumberStack() {
		this.top = null;
	}
	
	public boolean isEmpty() {
		if (top == null)
			return true;
		else return false;
	}
	
	public void push(double item) {
		NumberNode node = new NumberNode();
		node.item = item;
		node.next = top;
		top = node;
	}
	
	public double pop() {
		if(isEmpty() == true)
			return 0;
		else {
			NumberNode node = top;
			top = node.next;
			return node.item;
		}
	}
}

public class s20171665hw3 implements ActionListener{

	private Frame calculatorFrame;
	private JLabel statementLabel;
	private JPanel statementPanel, buttonPanel, buttonLeftPanel, buttonRightPanel;
	private String statement = "", postfixString = "";
	private double result = 0;
	private int resultInteger = 0;
	private int resultFlag = 0;
	private int resultIntegerFlag = 0;
	
	public void paint(Graphics g) {
		g.setColor(Color.BLACK);
		g.fillRect(20, 20, 200, 200);
	}
	
	public s20171665hw3() {
		calculatorFrame = new Frame("Calculator");
		calculatorFrame.setBackground(new Color(0xF5F5DC));
		
		statementLabel = new JLabel();
		statementLabel.setHorizontalAlignment(SwingConstants.RIGHT);
		Border statementBorder = new LineBorder(Color.red);
		Border labelMargin = new EmptyBorder(10, 10, 10, 10);
		statementLabel.setFont(new Font("Sefit", Font.PLAIN, 20));
		statementLabel.setOpaque(true);
		statementLabel.setBackground(Color.WHITE);
		statementLabel.setBorder(new CompoundBorder(statementBorder, labelMargin));

		statementPanel = new JPanel();
		Border statementPanelMargin = BorderFactory.createLineBorder(new Color(0xF5F5DC), 20);
		statementPanel.setBorder(statementPanelMargin);
		statementPanel.setLayout(new GridLayout(1, 1));
		statementPanel.add(statementLabel);
		
		buttonPanel = new JPanel();
		buttonPanel.setBackground(new Color(0xF5F5DC));
		Border buttonBorder =buttonPanel.getBorder();
		Border buttonPanelMargin = new EmptyBorder(20, 20, 20, 20);
		buttonPanel.setBorder(new CompoundBorder(buttonBorder, buttonPanelMargin));
		buttonPanel.setLayout(new GridLayout(1, 2, 10, 10));
		
		buttonLeftPanel = new JPanel();
		buttonLeftPanel.setBackground(new Color(0xF5F5DC));
		buttonLeftPanel.setLayout(new GridLayout(4,3,10,10));
		
		buttonRightPanel = new JPanel();
		buttonRightPanel.setBackground(new Color(0xF5F5DC));
		buttonRightPanel.setLayout(new GridLayout(4,2,10,10));
		
		JButton[] numberButton = new JButton[10];
		JButton[] operatorButton = new JButton[10];
		
		for(int i = 0; i < 10; i++) {
			numberButton[i] = new JButton(String.valueOf(i));
		}

		operatorButton[0] = new JButton(".");
		operatorButton[1] = new JButton("=");
		operatorButton[2] = new JButton("/");
		operatorButton[3] = new JButton("C");
		operatorButton[4] = new JButton("*");
		operatorButton[5] = new JButton("<-");
		operatorButton[6] = new JButton("-");
		operatorButton[7] = new JButton("(");
		operatorButton[8] = new JButton("+");
		operatorButton[9] = new JButton(")");
		
		
		for(int i = 0; i < 10; i++) {
			numberButton[i].addActionListener(this);
			operatorButton[i].addActionListener(this);
			numberButton[i].setFont(new Font("Sefit", Font.PLAIN, 20));
			numberButton[i].setForeground(Color.BLUE);
			operatorButton[i].setFont(new Font("Sefit", Font.PLAIN, 20));
			if(i != 0) {
				operatorButton[i].setForeground(Color.RED);
			}
			else {
				operatorButton[i].setForeground(Color.BLUE);
			}
			buttonLeftPanel.add(numberButton[i]);
		}
		
		buttonLeftPanel.add(operatorButton[0]);
		buttonLeftPanel.add(operatorButton[1]);
		
		for(int i = 2; i < 10; i++) {
			buttonRightPanel.add(operatorButton[i]);
		}
		
		buttonPanel.add(buttonLeftPanel);
		buttonPanel.add(buttonRightPanel);
	
		calculatorFrame.setLayout(new GridLayout(2, 1, 10, 10));
		calculatorFrame.add(statementPanel);
		calculatorFrame.add(buttonPanel);
		calculatorFrame.pack();
		
		calculatorFrame.setSize(600, 800);
		WindowDestroyer listener = new WindowDestroyer();
		calculatorFrame.addWindowListener(listener);
		calculatorFrame.setVisible(true);
	}
	
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().charAt(0) >= 48 && e.getActionCommand().charAt(0) <= 57) {
			if(resultFlag == 1) {
				result = 0;
			}
			else if(resultIntegerFlag == 1) {
				resultInteger = 0;
			}
			statement = statement + e.getActionCommand();
			statementLabel.setText(statement);
		}
		else {
			if(resultFlag == 1) {
				statement = "" + result;
				resultFlag = 0;
			}
			else if(resultIntegerFlag == 1) {
				statement = "" + resultInteger;
				resultIntegerFlag = 0;
			}
			int operatorFlag = 0;
			if(statement.length() == 0) {
				statement = "0";
			}
			if(statement.length() > 0) {
				switch(statement.charAt(statement.length() - 1)) {
				case '+':
				case '-':
				case '*':
				case '/':
				case '.':
					operatorFlag = 1;
					break;
				}
				switch(e.getActionCommand()) {
				case "+": 
				case "-": 
				case "*":
				case "/":
				case ".": 
					if(operatorFlag == 1) {
						statement = statement.substring(0, statement.length() - 1);
					}
					statement = statement + e.getActionCommand();
					statementLabel.setText(statement);
					break;
				case "(":
					if(statement == "0") {
						statement = "";
					}
					statement = statement + e.getActionCommand();
					statementLabel.setText(statement);
					break;
				case ")":
					if(statement == "0") {
						statement = "";
						break;
					}
					if(statement.charAt(statement.length() - 1) == '(') {
						statement = statement + "0";
					}
					statement = statement + e.getActionCommand();
					statementLabel.setText(statement);
					break;
				case "<-": statement = statement.substring(0, statement.length() - 1);
					statementLabel.setText(statement);
					break;
				case "C": 
					if(statement.length() > 0) {
						statement = "";
						statementLabel.setText(statement);
					}
					break;
				case "=": statement = statement + e.getActionCommand();
					// 계산 과정에서 POSTFIX로 바꾸는 작업 시행하기
					changeToPostfix(statement);
					evaluatePostfix(postfixString);
					if(result == (int)result) {
						resultInteger = (int)result;
						statementLabel.setText("<html><div align=right>" + statement + "<br/>" + resultInteger + "</div></html>");
						resultIntegerFlag = 1;
					}
					else {
						statementLabel.setText("<html><div align=right>" + statement + "<br/>" + result  + "</div></html>");
						resultFlag = 1;
					}
					postfixString = "";
					break;	
				}
			}
		}
	}
	
	public int stackPopRank(char operator) {
		int resultRank = -1;
		switch(operator) {
		case '(': resultRank = 0; break;
		case ')': resultRank = 19; break;
		case '+': resultRank = 12; break;
		case '-': resultRank = 12; break;
		case '*': resultRank = 12; break;
		case '/': resultRank = 12; break;
		case '\0': resultRank = 0; break;
		}
		return resultRank;
	}
	
	public int stackPushRank(char operator) {
		int resultRank = -1;
		switch(operator) {
		case '(': resultRank = 20; break;
		case ')': resultRank = 19; break;
		case '+': resultRank = 12; break;
		case '-': resultRank = 12; break;
		case '*': resultRank = 12; break;
		case '/': resultRank = 12; break;
		case '\0': resultRank = 0; break;
		}
		return resultRank;
	}
	
	public void changeToPostfix(String infix) {
		OperatorStack operatorStack = new OperatorStack();
		operatorStack.push('\0');
		char tempCharacter = '\0'; 
		
		for (int i = 0; i < infix.length(); i++) {
			if((infix.charAt(i) >= 48 && infix.charAt(i) <= 57) || (infix.charAt(i) == '.')) {
				postfixString = postfixString +  infix.charAt(i);
			}
			else {
				postfixString = postfixString + " ";
				switch(infix.charAt(i)) {
				case '+':
				case '-':
				case '/':
				case '*':
				case '(':
					while(stackPopRank(operatorStack.peek()) >= stackPushRank(infix.charAt(i))) {
						tempCharacter = operatorStack.pop();
						postfixString = postfixString + tempCharacter + " ";
					}
					operatorStack.push(infix.charAt(i));
					break;
				case ')':
					while(operatorStack.peek() != '(' && operatorStack.peek() != '\0') {
						tempCharacter = operatorStack.pop();
						postfixString = postfixString + tempCharacter + " ";
					}
					operatorStack.pop();
					break;
				default: break;
				}
			}
			
		}
		while((tempCharacter = operatorStack.pop()) != '\0') {
			postfixString = postfixString + tempCharacter + " ";
		}
	}
	
	@SuppressWarnings("deprecation")
	public void evaluatePostfix(String postfix) {
		double number1 = 0, number2 = 0;
		double tempNumber = 0;
		double digit = 10;
		double floatDigit = 0.1;
		int floatFlag = 0;
		NumberStack numberStack = new NumberStack();
		
		for (int i = 0; i < postfix.length(); i++) {
			if(postfix.charAt(i) >= 48 && postfix.charAt(i) <= 57) {
				if(floatFlag == 0) {
					tempNumber = tempNumber * digit + (double)(postfix.charAt(i) - '0');
				}
				else if(floatFlag == 1) {
					BigDecimal tempFloat = new BigDecimal(Double.toString(tempNumber));
					BigDecimal tempFloatDigit = new BigDecimal(Double.toString(floatDigit));
					BigDecimal tempNum =  new BigDecimal(Double.toString((double)(postfix.charAt(i) - '0')));
					BigDecimal tempResult = new BigDecimal("0");
					
					tempResult = tempFloat.add(tempFloatDigit.multiply(tempNum));
					tempNumber = tempResult.doubleValue();
					
					floatDigit = floatDigit * 0.1;
				}
			}
			else if(postfix.charAt(i) == ' ') {
				if(tempNumber != 0) {
					numberStack.push(tempNumber);
				}
				tempNumber = 0;
				floatFlag = 0;
				digit = 10;
				floatDigit = 0.1;
			}
			else if(postfix.charAt(i) == '.') {
				floatFlag = 1;
			}
			else if(postfix.charAt(i) == '+' || postfix.charAt(i) == '-' || postfix.charAt(i) == '*' || postfix.charAt(i) == '/') {
				number2 = numberStack.pop();
				number1 = numberStack.pop();
				
				BigDecimal b1 = new BigDecimal(Double.toString(number1));
				BigDecimal b2 = new BigDecimal(Double.toString(number2));
				BigDecimal bResult = new BigDecimal("0");
				
				switch(postfix.charAt(i)) {
				case '+':
					bResult = b1.add(b2);
					numberStack.push(bResult.doubleValue());
					break;
				case '-':
					bResult = b1.subtract(b2);
					numberStack.push(bResult.doubleValue());
					break;
				case '*':
					bResult = b1.multiply(b2, MathContext.DECIMAL64);
					numberStack.push(bResult.doubleValue());
					break;
				case '/':
					bResult = b1.divide(b2, 16, BigDecimal.ROUND_FLOOR);
					numberStack.push(bResult.doubleValue());
					break;
				}
			}
		}
		result = numberStack.pop();
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		s20171665hw3 calculator = new s20171665hw3();
	}

}
