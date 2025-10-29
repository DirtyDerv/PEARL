"""
OpenAI Provider (including Codex)
"""

import asyncio
from typing import Optional
from .base import BaseProvider

try:
    import openai
except ImportError:
    openai = None


class OpenAIProvider(BaseProvider):
    """OpenAI provider for GPT models and Codex"""
    
    def __init__(self, api_key: str, model: str = "gpt-4", **kwargs):
        super().__init__(api_key, model, **kwargs)
        self.client = None
        if openai:
            self.client = openai.OpenAI(api_key=api_key)
    
    @property
    def provider_name(self) -> str:
        return "OpenAI"
    
    async def generate_response(self, prompt: str, **kwargs) -> str:
        """Generate response using OpenAI"""
        if not self.client:
            raise RuntimeError("OpenAI client not available. Install with: pip install openai")
        
        try:
            response = await asyncio.to_thread(
                self.client.chat.completions.create,
                model=self.model,
                max_tokens=kwargs.get('max_tokens', 4000),
                temperature=kwargs.get('temperature', 0.7),
                messages=[
                    {"role": "user", "content": prompt}
                ]
            )
            return response.choices[0].message.content
        except Exception as e:
            return f"Error from OpenAI: {str(e)}"
    
    def validate_connection(self) -> bool:
        """Validate OpenAI API connection"""
        if not self.client:
            return False
        
        try:
            # Try a simple completion to validate the key
            response = self.client.chat.completions.create(
                model=self.model,
                max_tokens=10,
                messages=[{"role": "user", "content": "Hi"}]
            )
            return response.choices[0].message.content is not None
        except Exception:
            return False
