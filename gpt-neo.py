from transformers import GPTNeoForCausalLM, GPT2Tokenizer

# Load GPT-Neo model and tokenizer
model = GPTNeoForCausalLM.from_pretrained("EleutherAI/gpt-neo-2.7B")
tokenizer = GPT2Tokenizer.from_pretrained("EleutherAI/gpt-neo-2.7B")

# Generate text
prompt = "Once upon a time"
inputs = tokenizer(prompt, return_tensors="pt")
outputs = model.generate(inputs['input_ids'], max_length=100)

# Print the generated text
generated_text = tokenizer.decode(outputs[0], skip_special_tokens=True)
print(generated_text)

