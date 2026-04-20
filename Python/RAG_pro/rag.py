from langchain_core.output_parsers import StrOutputParser
from vector_stores import VectorStoreService
from langchain_community.embeddings import DashScopeEmbeddings
import config_data as config
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder
from langchain_core.runnables import RunnablePassthrough, RunnableWithMessageHistory, RunnableLambda
from langchain_core.documents import Document
from file_history_store import get_history

import os
from dotenv import load_dotenv
load_dotenv()
from langchain.chat_models import init_chat_model
model = init_chat_model(
    model = "qwen3.6-flash",
    model_provider="openai",
    base_url=os.getenv("DASHSCOPE_BASE_URL"),
    api_key=os.getenv("DASHSCOPE_API_KEY"),
)

def print_prompt(prompt):
    print("=" * 20)
    print(prompt)
    print("=" * 20)
    return prompt

class RagService(object):
    def __init__(self):
        self.vector_service = VectorStoreService(
            embedding=DashScopeEmbeddings(model=config.embedding_model_name)
        )
        self.prompt_template = ChatPromptTemplate.from_messages(
            [
                ("system", "以我提供的已知参考资料为主，"
                "简洁和专业的回答用户问题。参考资料：{context}。"),
                ("system","这里提供用户的历史对话"),
                MessagesPlaceholder("history"),
                ("user", "请回答用户提问：{input}")
            ]
        )
        self.chat_model = model
        self.chain = self.__get__chain()

    def __get__chain(self):
        # 检索器）去向量数据库查找相关的文档
        retriever = self.vector_service.get_retriever()

        def format_document(docs : list[Document]):
            if not docs:
                return "无相关资料"
            formatted_str = ""
            for doc in docs:
                formatted_str += f"文档片段：{doc.page_content}\n文档元数据：{doc.metadata}\n\n"
            return formatted_str

        def temp1(v : dict) -> str:
            return v["input"]

        def temp2(v):
            new_value = {}
            new_value["input"] = v["input"]["input"]
            new_value["context"] = v["context"]
            new_value["history"] = v["input"]["history"]
            return new_value

        chain = (
            {
                "input": RunnablePassthrough(),
                "context": RunnableLambda(temp1) | retriever | format_document
            } | RunnableLambda(temp2) | self.prompt_template | print_prompt | self.chat_model | StrOutputParser()
        )

        conversation_chain = RunnableWithMessageHistory(
            chain,
            get_history,
            input_messages_key="input",
            history_messages_key="history"
        )

        return conversation_chain

if __name__ == '__main__':
    session_config = {
        "configurable":{
            "session_id":"user_001"
        }
    }
    res = RagService().chain.invoke({"input":"不好，重新来"}, session_config)
    print(res)